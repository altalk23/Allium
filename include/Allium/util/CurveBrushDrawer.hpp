#pragma once

#include "BrushDrawer.hpp"

namespace allium {

    class CurveOptimizer;
    class CurveBrushDrawer : public BrushDrawer {
    protected:
        // Maximum size of 4 points
        std::vector<cocos2d::CCPoint> m_points;

        // Todo: Implement adjustable line width
        float m_lineWidth = 5.0f;
        // Todo: Implement adjustable line color
        cocos2d::ccColor3B m_lineColor = cocos2d::ccc3(255, 255, 255);
        // Todo: Implement adjustable curve roughness
        float m_curveRoughness = 0.4f;

        // Generated points for all of the curves
        std::vector<std::array<double, 2>> m_previousPoints;
        std::vector<std::array<double, 2>> m_currentPoints;

    private:
        std::vector<std::array<double, 2>> getGeneratedPoints();
        CurveOptimizer initalizeOptimizer();

    public:
        static CurveBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        void updateLine() override;
    };
}