#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    class EditorUtilities {
    protected:
        std::vector<GameObject*> m_objects;
        int m_colorID = 0;
    public: 
        static EditorUtilities* get();

        void setColor(int colorID);

        void addRectangle(
            cocos2d::CCPoint const& p1, cocos2d::CCPoint const& p2, cocos2d::CCPoint const& p3, cocos2d::CCPoint const& p4
        );

        void addCircle(
            cocos2d::CCPoint const& center, float radius
        );

        void finish();
    };
}