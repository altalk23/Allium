#pragma once

#include <Geode/Geode.hpp>
#include "PolylineConverter.hpp"

namespace allium {
    class BrushDrawer : public cocos2d::CCNode {
    protected:
        cocos2d::CCDrawNode* m_overlay = nullptr;
        bool m_canUpdateLine = false;

    public:
        bool init() override;

        virtual bool handleTouchStart(cocos2d::CCPoint const& point);
        virtual void handleTouchMove(cocos2d::CCPoint const& point);
        virtual void handleTouchEnd(cocos2d::CCPoint const& point);

        virtual void updateOverlay();
        void clearOverlay();

        virtual void updateLine();

        virtual PolylineConverter initializeConverter() = 0;
    };
}