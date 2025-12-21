#pragma once

#include "BrushDrawer.hpp"
#include "../data/Geometry.hpp"

namespace allium {

    class CurveOptimizer;
    class CurveBrushDrawer : public BrushDrawer {
    protected:
        // Maximum size of 4 points
        std::vector<Point> m_points;

        // Generated points for all of the curves
        std::vector<Point> m_previousPoints;
        std::vector<Point> m_currentPoints;

    private:
        std::vector<Point> getGeneratedPoints();

    public:
        static CurveBrushDrawer* create();
        bool init() override;

        bool handleTouchStart(cocos2d::CCPoint const& point) override;
        void handleTouchMove(cocos2d::CCPoint const& point) override;
        void handleTouchEnd(cocos2d::CCPoint const& point) override;

        void updateOverlay() override;

        bool usesDeadzone() override { return true; }

        void updateLine() override;

        std::unique_ptr<BaseConverter> initializeConverter() override;
    };
}