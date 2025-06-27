#pragma once

#include <Geode/Geode.hpp>

namespace allium {
    // the EditButtonBar expects all children to be CCMenuItemSpriteExtra and will create a crash otherwise
    // this is intended to be a quick replacement for CCMenuItemExt::createToggler
    // thanks to https://github.com/hiimjasmine00/BetterEdit/blob/b505161a0bee587f6241201198b1587540ac9e06/src/features/ViewTab/ViewTab.cpp#L25 :)
    class MenuItemTogglerExtra : public CCMenuItemSpriteExtra {
    protected:
        std::function<void(MenuItemTogglerExtra*)> m_callback;
        geode::Ref<cocos2d::CCNode> m_onSprite;
        geode::Ref<cocos2d::CCNode> m_offSprite;
        bool m_toggled = false;

        void onCallback(cocos2d::CCObject*);
        void updateToggleSprite();

        bool init(
            cocos2d::CCNode* onSprite, cocos2d::CCNode* offSprite,
            std::function<void(MenuItemTogglerExtra*)> callback
        );

    public:
        static MenuItemTogglerExtra* create(
            cocos2d::CCNode* onSprite, cocos2d::CCNode* offSprite,
            std::function<void(MenuItemTogglerExtra*)> callback
        );

        void toggle(bool);
        bool isToggled() const;
    };
}
