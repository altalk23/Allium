#pragma once

#include <Geode/Geode.hpp>
#include "BrushDrawer.hpp"

namespace allium {
    class FreeBrushDrawer : public BrushDrawer {
    protected:
        std::vector<cocos2d::CCPoint> m_points;
    private:

        std::vector<cocos2d::CCPoint> simplify(std::vector<cocos2d::CCPoint> const& points);
    public:
        static FreeBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        PolylineConverter initializeConverter() override;
    };
}