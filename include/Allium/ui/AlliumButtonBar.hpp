#pragma once

#include <Geode/Geode.hpp>
#include "../util/BrushDrawer.hpp"

namespace allium {
    class BrushDrawer;
    class AlliumButtonBar : public cocos2d::CCNode {
    protected:
        geode::Ref<EditButtonBar> m_buttonBar;
        geode::Ref<cocos2d::CCArray> m_buttons = nullptr;

        geode::Ref<CCMenuItemToggler> m_lineToggle = nullptr;
        geode::Ref<CCMenuItemToggler> m_curveToggle = nullptr;
        geode::Ref<CCMenuItemToggler> m_freeToggle = nullptr;
        geode::Ref<CCMenuItemToggler> m_polygonToggle = nullptr;
        geode::Ref<CCMenuItemToggler> m_textToggle = nullptr;
        geode::Ref<BrushDrawer> m_brushDrawer = nullptr;

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