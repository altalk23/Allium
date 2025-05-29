#pragma once

#include "BrushDrawer.hpp"

namespace allium {
    class TextBrushDrawer : public BrushDrawer {
    protected:
        std::vector<Point> m_points;
        std::u32string m_text;
    public:
        static TextBrushDrawer* create(std::u32string const& text = U"");
        bool init(std::u32string const& text = U"");

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        std::unique_ptr<BaseConverter> initializeConverter() override;
    };
}