#pragma once

#include "BrushDrawer.hpp"

namespace allium {
    class PolygonBrushDrawer : public BrushDrawer {
    protected:
        std::vector<Point> m_points;
    public:
        static PolygonBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        std::unique_ptr<BaseConverter> initializeConverter() override;
    };
}