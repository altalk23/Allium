#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    class DrawNodeExtension : public cocos2d::CCDrawNode {
    public:
        static DrawNodeExtension* create();

        void ensureCapacity(unsigned int count);
        void drawPolygon(cocos2d::CCPoint *verts, unsigned int count, const cocos2d::ccColor4F &fillColor);
    };
}