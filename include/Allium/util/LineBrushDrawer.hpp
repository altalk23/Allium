#pragma once

#include "BrushDrawer.hpp"

namespace allium {
    class LineBrushDrawer : public BrushDrawer {
    protected:
        Point m_firstPoint;
        Point m_lastPoint;
    public:
        static LineBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        std::unique_ptr<BaseConverter> initializeConverter() override;
    };
}