#include <ui/MenuItemTogglerExtra.hpp>

using namespace allium;
using namespace geode::prelude;

MenuItemTogglerExtra* MenuItemTogglerExtra::create(
    cocos2d::CCNode* onSprite, cocos2d::CCNode* offSprite,
    std::function<void(MenuItemTogglerExtra*)> callback
) {
    auto ret = new (std::nothrow) MenuItemTogglerExtra();
    if (ret && ret->init(onSprite, offSprite, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void MenuItemTogglerExtra::onCallback(cocos2d::CCObject*) {
    m_callback(this);
    toggle(!m_toggled);
}

bool MenuItemTogglerExtra::init(
    cocos2d::CCNode* onSprite, cocos2d::CCNode* offSprite,
    std::function<void(MenuItemTogglerExtra*)> callback
) {
    if (!CCMenuItemSpriteExtra::init(
        offSprite, nullptr, this,
        static_cast<SEL_MenuHandler>(&MenuItemTogglerExtra::onCallback))
    ) {
        return false;
    }

    m_callback = callback;
    m_onSprite = onSprite;
    m_offSprite = offSprite;

    return true;
}

void MenuItemTogglerExtra::updateToggleSprite() {
    auto sprite = m_toggled ? m_onSprite : m_offSprite;

    // why does setSprite take a sprite when it only needs a node
    this->setNormalImage(*sprite);
    this->updateSprite();
}

void MenuItemTogglerExtra::toggle(bool toggled) {
    m_toggled = toggled;
    this->updateToggleSprite();
}

bool MenuItemTogglerExtra::isToggled() const {
    return m_toggled;
}
