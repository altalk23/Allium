#pragma once

#include <Geode/Geode.hpp>
#include "BrushDrawer.hpp"

namespace allium {
    class FreeBrushDrawer : public BrushDrawer {
    protected:
        std::vector<Point> m_points;
        
    public:
        static FreeBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        std::unique_ptr<BaseConverter> initializeConverter() override;
    };
}