#pragma once

#include "BrushDrawer.hpp"

namespace allium {
    class LineBrushDrawer : public BrushDrawer {
    protected:
        cocos2d::CCPoint m_firstPoint = ccp(0, 0);
        cocos2d::CCPoint m_lastPoint = ccp(0, 0);

        // Todo: Implement adjustable line width
        float m_lineWidth = 5.0f;
        // Todo: Implement adjustable line color
        cocos2d::ccColor3B m_lineColor = cocos2d::ccc3(255, 255, 255);

    public:
        static LineBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;
    };
}