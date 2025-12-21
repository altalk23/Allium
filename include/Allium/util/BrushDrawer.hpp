#pragma once

#include <Geode/Geode.hpp>
#include "converter/BaseConverter.hpp"
#include "DrawNodeExtension.hpp"

namespace allium {
    class BrushDrawer : public cocos2d::CCNode {
    protected:
        DrawNodeExtension* m_overlay = nullptr;
        bool m_canUpdateLine = false;

    public:
        bool init() override;

        virtual bool handleTouchStart(cocos2d::CCPoint const& point);
        virtual void handleTouchMove(cocos2d::CCPoint const& point);
        virtual void handleTouchEnd(cocos2d::CCPoint const& point);

        virtual void updateOverlay();
        void clearOverlay();
        bool isOverlayVisible() const;

        virtual void updateLine();

        virtual std::unique_ptr<BaseConverter> initializeConverter() = 0;
    };
}