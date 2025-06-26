#pragma once

#include "BrushDrawer.hpp"
#include <alk.ime-input/include/IMEExtension.hpp>
#include <cocos2d.h>

namespace TTFFontParser {
    struct FontData;
}

namespace allium {
    class TextBrushDrawer : public BrushDrawer, public cocos2d::CCIMEDelegate, public ime::IMEExtensionDelegate {
    protected:
        std::u32string m_text;
        std::string m_u8text;
        std::u32string m_composition;
        std::vector<std::u32string> m_candidates;
        size_t m_candidateIndex = 0;

        std::vector<cocos2d::CCRect> m_boxes;
        size_t m_cursor = 0;
        Point m_cursorPos;
        Point m_endPos;
        double m_lineHeight = 0.0;
        double m_fontScale = 1.0;

        bool m_isFocused = false;
        bool m_updatedOrigin = false;
        Point m_origin;

        std::filesystem::path m_fontPath;
        std::unique_ptr<TTFFontParser::FontData> m_fontData;

        void relocateCursor(cocos2d::CCPoint const& point, bool updateIME);
        std::unique_ptr<BaseConverter> initializeConverterFor(std::u32string const& text, Point const& pos);

    public:
        static TextBrushDrawer* create();
        bool init() override;

        void initFontData();

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        std::unique_ptr<BaseConverter> initializeConverter() override;

        bool attachWithIME() override;
        bool detachWithIME() override;
        bool canAttachWithIME() override;
        bool canDetachWithIME() override;
        void insertText(char const* text, int len, cocos2d::enumKeyCodes) override;
        char const* getContentText() override;
        void deleteBackward() override;
        void deleteForward() override;
        void candidateList(std::vector<std::u32string> const& candidates, size_t currentCandidate) override;
        void composition(std::u32string const& compositionString) override;
    };
}