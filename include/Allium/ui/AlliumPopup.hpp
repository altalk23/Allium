#pragma once

#include <Geode/ui/Popup.hpp>

namespace allium {
    class AlliumPopup : public geode::Popup<> {
    protected:
        cocos2d::CCNode* m_mainColumn = nullptr;
        cocos2d::CCMenu* m_brushMenu = nullptr;

        CCMenuItemToggler* m_noneBrushToggle = nullptr;
        CCMenuItemToggler* m_lineBrushToggle = nullptr;
        CCMenuItemToggler* m_curveBrushToggle = nullptr;
        CCMenuItemToggler* m_freeBrushToggle = nullptr;

    public:
        static AlliumPopup* create();

        cocos2d::CCNode* getEnablePanningSprite();
        cocos2d::CCNode* getDisablePanningSprite();

        void createPanButton();
        void createFinalizeButton();

        void brushToggleCallback(CCMenuItemToggler* toggle);

        void createBrushToggle(
            std::string_view name, std::string const& id, CCMenuItemToggler*& toggle
        );

        bool setup() override;
    };
}