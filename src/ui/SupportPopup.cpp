#include <ui/SupportPopup.hpp>

using namespace allium;
using namespace geode::prelude;

SupportPopup* SupportPopup::create() {
    auto ret = new (std::nothrow) SupportPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

SupportPopup::~SupportPopup() {
    cocos2d::CCTouchDispatcher::get()->unregisterForcePrio(this);
}

void SupportPopup::registerWithTouchDispatcher() {
    cocos2d::CCTouchDispatcher::get()->addTargetedDelegate(this, -500, true);
}

bool SupportPopup::init() {
    if (!FLAlertLayer::initWithColor({0, 0, 0, 160})) return false;

    this->setAnchorPoint(ccp(0, 0));
    this->setContentSize(CCDirector::sharedDirector()->getWinSize());

    this->setKeypadEnabled(true);

    cocos2d::CCTouchDispatcher::get()->registerForcePrio(this, 2);

    m_mainLayer = CCLayer::create();
    this->addChildAtPosition(m_mainLayer, Anchor::Center, ccp(0, 0));

    auto sprite = CCSprite::create("SupportPopup.png"_spr);
    m_mainLayer->addChildAtPosition(sprite, Anchor::Center, ccp(0, 0));

    m_buttonMenu = CCMenu::create();
    m_buttonMenu->setContentSize(sprite->getContentSize());
    sprite->addChildAtPosition(m_buttonMenu, Anchor::Center, ccp(0, 0));

    auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    auto closeBtn = CCMenuItemSpriteExtra::create(closeSprite, closeSprite, this, menu_selector(SupportPopup::onClose));
    m_buttonMenu->addChildAtPosition(closeBtn, Anchor::TopLeft, ccp(10, -10));

    auto buttonSprite = ButtonSprite::create("Support!", "bigFont.fnt", "GiftButton.png"_spr, 0.7f);
    auto button = CCMenuItemExt::createSpriteExtra(buttonSprite, 
        [](auto sender) {
            Mod::get()->setSavedValue<bool>("allium-gift-popup-shown", true);
            CCApplication::sharedApplication()->openURL("https://ko-fi.com/alk1m123");
        }
    );
    m_buttonMenu->addChildAtPosition(button, Anchor::Bottom, ccp(0, 0));

    return true;
}

void SupportPopup::keyBackClicked() {
    this->removeFromParent();
}

void SupportPopup::onClose(CCObject* sender) {
    this->removeFromParent();
}