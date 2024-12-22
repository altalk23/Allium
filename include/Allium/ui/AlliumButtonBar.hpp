#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    class BrushDrawer;
    class AlliumButtonBar : public cocos2d::CCNode {
    protected:
        geode::Ref<EditButtonBar> m_buttonBar;
        cocos2d::CCArray* m_buttons = nullptr;

        CCMenuItemToggler* m_lineToggle = nullptr;
        CCMenuItemToggler* m_curveToggle = nullptr;
        CCMenuItemToggler* m_freeToggle = nullptr;
        BrushDrawer* m_brushDrawer = nullptr;

    public:
        static AlliumButtonBar* create(EditorUI* editorUI);

        bool init(EditorUI* editorUI);

        void resetToggles(cocos2d::CCObject* sender);

        EditButtonBar* getButtonBar() const;
        BrushDrawer* getBrushDrawer() const;

        CCMenuItemSpriteExtra* addButton(
            std::string_view spriteName, std::string_view bgName, std::string_view id, 
            std::function<void(CCMenuItemSpriteExtra*)> const& callback
        );

        CCMenuItemSpriteExtra* addDefaultButton(
            std::string_view spriteName, std::string_view id, 
            std::function<void(CCMenuItemSpriteExtra*)> const& callback
        );

        CCMenuItemToggler* addToggle(
            std::string_view spriteName, std::string_view bgOnName, std::string_view bgOffName, std::string_view id, 
            std::function<void(CCMenuItemToggler*)> const& callback
        );

        CCMenuItemToggler* addDefaultToggle(
            std::string_view spriteName, std::string_view id, 
            std::function<void(CCMenuItemToggler*)> const& callback
        );
    };
}