#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.editortab_api/ui/EditorTab.hpp>

#include "MenuItemTogglerExtra.hpp"

namespace allium {
    class BrushDrawer;
}

namespace allium {
    class AlliumButtonBar : public cocos2d::CCNode {
    protected:
        geode::Ref<alpha::editor_tabs::EditorTab> m_editorTab;
        std::vector<geode::Ref<cocos2d::CCNode>> m_buttons;

        MenuItemTogglerExtra* m_lineToggle = nullptr;
        MenuItemTogglerExtra* m_curveToggle = nullptr;
        MenuItemTogglerExtra* m_freeToggle = nullptr;
        MenuItemTogglerExtra* m_polygonToggle = nullptr;
        MenuItemTogglerExtra* m_textToggle = nullptr;
        BrushDrawer* m_brushDrawer = nullptr;

    public:
        static AlliumButtonBar* create(EditorUI* editorUI);

        bool init(EditorUI* editorUI);

        void resetToggles(cocos2d::CCObject* sender);

        EditButtonBar* getButtonBar() const;
        alpha::editor_tabs::EditorTab* getEditorTab() const;
        BrushDrawer* getBrushDrawer() const;

        CCMenuItemSpriteExtra* addButton(
            std::string_view spriteName, std::string_view bgName, std::string_view id, 
            std::function<void(CCMenuItemSpriteExtra*)> const& callback
        );

        CCMenuItemSpriteExtra* addDefaultButton(
            std::string_view spriteName, std::string_view id, 
            std::function<void(CCMenuItemSpriteExtra*)> const& callback
        );

        MenuItemTogglerExtra* addToggle(
            std::string_view spriteName, std::string_view bgOnName, std::string_view bgOffName, std::string_view id, 
            std::function<void(MenuItemTogglerExtra*)> const& callback
        );

        MenuItemTogglerExtra* addDefaultToggle(
            std::string_view spriteName, std::string_view id,
            std::function<void(MenuItemTogglerExtra*)> const& callback
        );
    };
}