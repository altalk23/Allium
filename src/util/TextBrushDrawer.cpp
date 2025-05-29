#include <util/TextBrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <util/converter/TriangulatorConverter.hpp>
#include <agg-2.6/agg_curves.h>
#define TTF_FONT_PARSER_IMPLEMENTATION
#include <ttfParser.h>

using namespace geode::prelude;
using namespace allium;

TextBrushDrawer* TextBrushDrawer::create(std::u32string const& text) {
    auto ret = new (std::nothrow) TextBrushDrawer();
    if (ret && ret->init(text)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool TextBrushDrawer::init(std::u32string const& text) {
    if (!BrushDrawer::init()) return false;

    m_text = text;

    return true;
}

TTFFontParser::FontData fontData;
bool inited = false;

bool TextBrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    if (!inited) {
        auto binaryData = file::readBinary(CCFileUtils::sharedFileUtils()->fullPathForFilename("Roboto-Regular.ttf"_spr, true));
        if (binaryData) {
            int error = TTFFontParser::parse_data((const char*)binaryData.unwrap().data(), &fontData);
            if (error == 0) {
                inited = true;
                log::debug("TTF font data parsed successfully");

                log::debug("Font family: {}", fontData.font_names.begin()->font_family);
                log::debug("Font style: {}", fontData.font_names.begin()->font_style);
                for (auto name : fontData.font_names) {
                    log::debug("Font name: {} (Platform ID: {}, Encoding ID: {}, Language ID: {})",
                        name.font_family, name.platformID, name.encodingID, name.languageID);
                }
                log::debug("Number of glyphs: {}", uint32_t(fontData.glyphs.size()));

                for (auto& [c, _] : fontData.glyphs) {
                    if (c >= 0x96e0 && c <= 0x96eF) {
                        log::debug("Glyph 0x{:x} found", c);
                    }
                }
            }
            else {
                log::error("Failed to parse TTF font data: error code {}", error);
            }
        }
        else {
            log::error("Failed to read TTF font data");
        }
    }
    // m_points.emplace_back(point);
    // if (m_points.size() >= 3) m_canUpdateLine = true;
    return true;
}
void TextBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    // if (m_points.size() > 0) m_points.back() = point;
    this->updateOverlay();
}
void TextBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    // if (m_points.size() > 0) m_points.back() = point;
    this->clearOverlay();
    m_canUpdateLine = true;
    this->updateLine();
}

std::unique_ptr<BaseConverter> TextBrushDrawer::initializeConverter() {
    std::vector<TriangulatorConverter::Polygon> polygons;

    Point globalPos;
    int32_t lastCharacter = 0;
    // get scale from unitsPerEm
    auto const characterScale = 24.0 / fontData.meta_data.unitsPerEm;
    for (auto character32 : m_text) {
        auto character = static_cast<uint32_t>(character32);
        if (character == '\n') {
            globalPos.x = 0;
            globalPos.y -= fontData.meta_data.Ascender + fontData.meta_data.LineGap;
            continue;
        }
        auto glyph_iterator = fontData.glyphs.find(character);
        if (glyph_iterator == fontData.glyphs.end()) {
            log::error("Glyph 0x{:x} not found in font data", character);
            continue;
        }

        Point glyphPos = globalPos;
        polygons.emplace_back(); // Create a new polygon for each glyph

        glyphPos.x += glyph_iterator->second.left_side_bearing;
        auto const kerning = fontData.has_kearning_table ? fontData.kearning_table[uint64_t(lastCharacter) << 32 | character] : 0;
        glyphPos.x += kerning;

        uint32_t num_curves = 0, num_lines = 0;
        for (const auto& path_list : glyph_iterator->second.path_list) {
            polygons.back().emplace_back(); // Create a new contour for each path in the glyph
            for (const auto& geometry : path_list.geometry) {
                if (geometry.is_curve) {
                    // auto generator = agg::curve3_div();
                    // generator.approximation_scale(0.002f);
                    // generator.init(
                    //     geometry.p0.x + offsetX, geometry.p0.y + offsetY,
                    //     geometry.p1.x + offsetX, geometry.p1.y + offsetY,
                    //     geometry.c.x + offsetX, geometry.c.y + offsetY
                    // );

                    // double x, y;
                    // while (generator.vertex(&x, &y) != agg::path_cmd_stop) {
                    //     polygons.back().push_back(Point{x, y} * 0.05);
                    // }
                    // polygons.back().pop_back(); // Remove last point to avoid duplicate end point

                    // polygons.back().back().push_back(Point{geometry.p0.x, geometry.p0.y} + glyphPos);
                    num_curves++;
                }
                else {
                    num_lines++;
                    // polygons.back().back().push_back(Point{geometry.p0.x, geometry.p0.y} + glyphPos);
                }
                polygons.back().back().push_back((Point{geometry.p0.x, geometry.p0.y} + glyphPos) * characterScale);
            }
            // try simplifying the last contour
            polygons.back().back() = BaseConverter::simplify(polygons.back().back());
        }
        globalPos.x += glyph_iterator->second.advance_width;
        lastCharacter = character;
    }
    return std::make_unique<TriangulatorConverter>(
        std::move(polygons)
    );
}

void TextBrushDrawer::updateOverlay() {
    this->clearOverlay();
    
    BrushDrawer::updateOverlay();
}

void TextBrushDrawer::updateLine() {
    if (!m_canUpdateLine) return;
    m_canUpdateLine = false;
    
    BrushDrawer::updateLine();
}