#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    class BrushDrawer;

    class BrushManager {
    public:
        static BrushManager* get();

        cocos2d::ccColor3B getColor();

        int getColorID();
        float getLineWidth();
        float getCurveRoughness();
        float getFreeThreshold();

        void setColorID(int id);
        void setLineWidth(float width);
        void setCurveRoughness(float roughness);
        void setFreeThreshold(float threshold);

        bool m_panEditorInBrush = false;
        bool m_tempPanEditorInBrush = false;

        bool panEditorInBrush();
    };
}