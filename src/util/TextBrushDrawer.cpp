#include <util/TextBrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <util/converter/TriangulatorConverter.hpp>
#include <agg-2.6/agg_curves.h>
#include <Geode/loader/Dispatch.hpp>
#include <Geode/loader/Event.hpp>

#define TTF_FONT_PARSER_IMPLEMENTATION
#include <ttfParser.h>

using namespace geode::prelude;
using namespace allium;

TextBrushDrawer* TextBrushDrawer::create() {
    auto ret = new (std::nothrow) TextBrushDrawer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool TextBrushDrawer::init() {
    if (!BrushDrawer::init()) return false;

    this->initFontData();

    this->addChild(EventListenerNode<SettingChangedFilter>::create(EventListener(
        [this](auto event) {
            this->initFontData();
            return ListenerResult::Propagate;
        },
        SettingChangedFilter(Mod::get(), "font-path")
    )));

    this->addChild(EventListenerNode<SettingChangedFilter>::create(EventListener(
        [this](auto event) {
            this->initFontData();
            return ListenerResult::Propagate;
        },
        SettingChangedFilter(Mod::get(), "text-size")
    )));

    return true;
}

void TextBrushDrawer::initFontData() {
    m_fontPath = Mod::get()->getSettingValue<std::filesystem::path>("font-path");
    if (m_fontPath.empty()) {
        m_fontPath = Mod::get()->getResourcesDir() / "Nunito-Regular.ttf";
    }
    m_fontData = std::make_unique<TTFFontParser::FontData>();
    auto binaryData = file::readBinary(m_fontPath);
    if (binaryData) {
        int error = TTFFontParser::parse_data((const char*)binaryData.unwrap().data(), m_fontData.get());
        if (error == 0) {
            log::debug("TTF font data parsed successfully");
        }
        else {
            log::error("Failed to parse TTF font data: error code {}", error);
        }
    }
    m_fontScale = Mod::get()->getSettingValue<double>("text-size") / m_fontData->meta_data.unitsPerEm;
    m_lineHeight = (m_fontData->meta_data.Ascender - m_fontData->meta_data.Descender + m_fontData->meta_data.LineGap) * m_fontScale;
}

void TextBrushDrawer::relocateCursor(cocos2d::CCPoint const& point, bool updateIME) {
    bool containsCursor = false;
    double minDistance = std::numeric_limits<float>::max();
    size_t closestIndex = 0;

    for (size_t i = 0; i < m_boxes.size(); ++i) {
        auto const& box = m_boxes[i];
        auto const center = box.origin + box.size / 2;
        containsCursor = containsCursor || box.containsPoint(point);
        double distance = (point - center).getLength();
        if (distance < minDistance) {
            minDistance = distance;
            closestIndex = i;
        }
    }
    if (m_boxes.size() > 0) {
        auto const& box = m_boxes.back();
        auto const end = box.origin + CCSize{box.size.width, box.size.height / 2};
        double endDistance = (point - end).getLength();
        if (endDistance < minDistance) {
            closestIndex = m_boxes.size();
        }
    }
    if (minDistance < 30.f || containsCursor || m_boxes.empty()) {
        // attach if the distance is small enough
        m_isFocused = true;
        this->attachWithIME();
    }
    else {
        // detach if the distance is too large
        m_isFocused = false;
        this->detachWithIME();
    }
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    m_cursor = m_boxes.size();
#else
    m_cursor = closestIndex;
#endif

    this->updateOverlay();
}

bool TextBrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    if (m_updatedOrigin && m_composition.empty()) {
        this->relocateCursor(point, false);
    }
    return true;
}
void TextBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    if (m_updatedOrigin && m_composition.empty()) {
        this->relocateCursor(point, false);
    }
}
void TextBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    if (!m_updatedOrigin) {
        m_origin = Point{point.x, point.y};
        m_endPos = m_origin;
        m_updatedOrigin = true;

        if (m_fontPath != Mod::get()->getSettingValue<std::filesystem::path>("font-path")) {
            this->initFontData();
        }
    }
    
    if (m_composition.empty()) {
        this->relocateCursor(point, true);
    }
    m_canUpdateLine = true;
}

std::unique_ptr<BaseConverter> TextBrushDrawer::initializeConverterFor(std::u32string const& text, Point const& pos) {
    std::vector<TriangulatorConverter::Polygon> polygons;

    Point globalPos = pos;
    int32_t lastCharacter = 0;

    auto const textDetail = Mod::get()->getSettingValue<double>("text-detail-threshold");

    m_boxes.clear();
    for (auto character32 : text) {
        polygons.emplace_back(); // Create a new polygon for each glyph
        m_boxes.emplace_back(
            cocos2d::CCRect{
                static_cast<float>(globalPos.x),
                static_cast<float>(globalPos.y),
                0.f, static_cast<float>(m_lineHeight)
            }
        );
        
        auto character = static_cast<uint32_t>(character32);
        if (character == '\n') {
            globalPos.x = m_origin.x;
            globalPos.y -= m_lineHeight;
            continue;
        }
        if (character == '\r') {
            // Ignore carriage return
            continue;
        }

        auto glyph_iterator = m_fontData->glyphs.find(character);
        if (glyph_iterator == m_fontData->glyphs.end()) {
            log::error("Glyph 0x{:x} not found in font data", character);
            continue;
        }

        auto const lineWidth = glyph_iterator->second.advance_width * m_fontScale;

        Point glyphPos = globalPos;

        auto const kerning = m_fontData->has_kearning_table ? m_fontData->kearning_table[uint64_t(lastCharacter) << 32 | character] : 0;
        glyphPos.x += kerning * m_fontScale;

        uint32_t num_curves = 0, num_lines = 0;
        for (const auto& path_list : glyph_iterator->second.path_list) {
            polygons.back().emplace_back(); // Create a new contour for each path in the glyph
            for (const auto& geometry : path_list.geometry) {
                if (geometry.is_curve) {
                    auto generator = agg::curve3_div();
                    generator.approximation_scale(1.0);
                    generator.init(
                        geometry.p0.x, geometry.p0.y,
                        geometry.p1.x, geometry.p1.y,
                        geometry.c.x, geometry.c.y
                    );

                    double x, y;
                    while (generator.vertex(&x, &y) != agg::path_cmd_stop) {
                        auto const pos = Point{x, y} * m_fontScale;
                        polygons.back().back().push_back(pos + glyphPos);
                    }
                    polygons.back().back().pop_back(); // Remove last point to avoid duplicate end point
                }
                else {
                    auto const pos = Point{geometry.p0.x, geometry.p0.y} * m_fontScale;
                    polygons.back().back().push_back(pos + glyphPos);
                }
            }
        }

        globalPos.x += glyph_iterator->second.advance_width * m_fontScale;
        lastCharacter = character;

        m_boxes.back() = cocos2d::CCRect{
            static_cast<float>(glyphPos.x),
            static_cast<float>(glyphPos.y),
            static_cast<float>(lineWidth),
            static_cast<float>(m_lineHeight)
        };
    }
    m_endPos = globalPos;
    return std::make_unique<TriangulatorConverter>(
        std::move(polygons), textDetail, true
    );
}

std::unique_ptr<BaseConverter> TextBrushDrawer::initializeConverter() {
    return this->initializeConverterFor(m_text, m_origin);
}

void TextBrushDrawer::updateOverlay() {
    this->clearOverlay();
    
    auto text = m_text;
    text.insert(m_cursor, m_composition);
    std::u32string candidateText = U"";
    size_t candidateOffset = m_cursor + m_composition.size();
    if (m_candidateIndex < m_candidates.size()) {
        candidateText = U" (" + m_candidates[m_candidateIndex] + U") ";
        text.insert(candidateOffset, candidateText);
    }
    auto convertedObjects = this->initializeConverterFor(text, m_origin)->handleExtension();

    for (auto const& object : convertedObjects) {
        auto const i = object->idx;
        if (i >= m_cursor && i < m_cursor + m_composition.size()) {
            // Highlight the composition part
            (void)object->drawIntoDrawNode(m_overlay, ccc3(255, 0, 0));
        }
        else if (i >= candidateOffset && i < candidateOffset + candidateText.size()) {
            // Highlight the candidate part
            (void)object->drawIntoDrawNode(m_overlay, ccc3(255, 0, 255));
        }
        else {
            // Draw the rest normally
            (void)object->drawIntoDrawNode(m_overlay, BrushManager::get()->getColor());
        }
    }

    if (m_isFocused) {
        if (m_cursor < m_boxes.size()) {
            auto& box = m_boxes[m_cursor];
            m_cursorPos = box.origin;
        }
        else {
            m_cursorPos = m_endPos;
        }

        m_overlay->drawSegment(
            m_cursorPos + Point{0, m_fontData->meta_data.Ascender * m_fontScale},
            m_cursorPos + Point{0, 0},
            0.5f, ccc4FFromccc3B(ccc3(0, 255, 0))
        );
    }
}

void TextBrushDrawer::updateLine() {
    if (!m_canUpdateLine) return;
    m_canUpdateLine = false;
    
    BrushDrawer::updateLine();

    m_updatedOrigin = false;
    if (m_isFocused) {
        this->detachWithIME();
        m_isFocused = false;
    }
    m_text.clear();
    m_composition.clear();
    m_cursor = 0;
    m_boxes.clear();
    m_candidates.clear();
    m_candidateIndex = 0;
}

bool TextBrushDrawer::attachWithIME() {
    if (CCIMEDelegate::attachWithIME()) {
        CCEGLView::get()->setIMEKeyboardState(true);
        return true;
    }
    return false;
}
bool TextBrushDrawer::detachWithIME() {
    if (CCIMEDelegate::detachWithIME()) {
        CCEGLView::get()->setIMEKeyboardState(false);
        return true;
    }
    return false;
}
bool TextBrushDrawer::canAttachWithIME() {
    return true;
}
bool TextBrushDrawer::canDetachWithIME() {
    return true;
}
void TextBrushDrawer::insertText(char const* text, int len, cocos2d::enumKeyCodes code) {
    switch (code) {
        case enumKeyCodes::KEY_Left: {
            if (m_cursor > 0) {
                m_cursor -= 1;
                this->updateOverlay();
            }
            return;
        }
        case enumKeyCodes::KEY_Right: {
            if (m_cursor < m_text.size()) {
                m_cursor += 1;
                this->updateOverlay();
            }
            return;
        }
        case enumKeyCodes::KEY_Down: {
            bool foundNewline = false;
            for (size_t idx = m_cursor + 1; idx < m_boxes.size(); ++idx) {
                auto c = m_text[idx];
                if (c == '\n') {
                    if (foundNewline) {
                        m_cursor = idx + 1;
                        this->updateOverlay();
                        return;
                    }
                    foundNewline = true;
                }

                auto& origin = m_boxes[idx].origin;
                if (origin.y < m_cursorPos.y && origin.x >= m_cursorPos.x) {
                    m_cursor = idx;
                    this->updateOverlay();
                    return;
                }
            }
            m_cursor = m_boxes.size();
            this->updateOverlay();
            return;
        }
        case enumKeyCodes::KEY_Up: {
            bool foundNewline = false;
            for (size_t idx = m_cursor; idx > 0; --idx) {
                auto c = m_text[idx - 1];
                if (c == '\n') {
                    if (foundNewline) {
                        m_cursor = idx;
                        this->updateOverlay();
                        return;
                    }
                    foundNewline = true;
                }

                auto& origin = m_boxes[idx - 1].origin;
                if (origin.y > m_cursorPos.y && origin.x < m_cursorPos.x) {
                    m_cursor = idx - 1;
                    this->updateOverlay();
                    return;
                }
            }
            m_cursor = 0;
            this->updateOverlay();
            return;
        }
        default: {
            m_composition = U"";
            auto value = string::utf8ToUtf32(std::string_view{text, (size_t)len}).unwrapOrDefault();
        #if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
            value.erase(std::remove(value.begin(), value.end(), U'\n'), value.end());
        #endif
            m_text.insert(m_cursor, value);
            m_cursor += value.size();
            this->updateOverlay();
            return;
        }
    }
}
char const* TextBrushDrawer::getContentText() {
    m_u8text = string::utf32ToUtf8(m_text).unwrapOrDefault();
    return m_u8text.c_str();
}
void TextBrushDrawer::deleteBackward() {
    if (!m_text.empty() && m_cursor > 0) {
        m_text.erase(m_cursor - 1, 1);

        m_cursor -= 1;
        this->updateOverlay();
    }
}
void TextBrushDrawer::deleteForward() {
    if (!m_text.empty() && m_cursor < m_text.size()) {
        m_text.erase(m_cursor, 1);

        this->updateOverlay();
    }
}
void TextBrushDrawer::candidateList(std::vector<std::u32string> const& candidates, size_t currentCandidate) {
    m_candidates = candidates;
    m_candidateIndex = currentCandidate;
    this->updateOverlay();
}
void TextBrushDrawer::composition(std::u32string const& compositionString) {
    m_composition = compositionString;
    log::debug("Composition updated: {}", string::utf32ToUtf8(m_composition).unwrapOrDefault());
    this->updateOverlay();
}