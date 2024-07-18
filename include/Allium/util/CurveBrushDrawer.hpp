#pragma once

#include "BrushDrawer.hpp"

namespace allium {

    class CurveOptimizer;
    class CurveBrushDrawer : public BrushDrawer {
    protected:
        // Maximum size of 4 points
        std::vector<cocos2d::CCPoint> m_points;

        // Generated points for all of the curves
        std::vector<std::array<double, 2>> m_previousPoints;
        std::vector<std::array<double, 2>> m_currentPoints;

    private:
        std::vector<std::array<double, 2>> getGeneratedPoints();

    public:
        static CurveBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;

        PolylineConverter initializeConverter() override;
    };
}