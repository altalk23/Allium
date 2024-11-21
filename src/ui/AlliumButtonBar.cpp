#include <ui/AlliumButtonBar.hpp>
#include <manager/BrushManager.hpp>
#include <util/BrushDrawer.hpp>
#include <util/CurveBrushDrawer.hpp>
#include <util/FreeBrushDrawer.hpp>
#include <util/LineBrushDrawer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <ui/SupportPopup.hpp>

using namespace allium;
using namespace geode::prelude;

AlliumButtonBar* AlliumButtonBar::create(EditorUI* editorUI) {
    auto ret = new (std::nothrow) AlliumButtonBar();
    if (ret && ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AlliumButtonBar::init(EditorUI* editorUI) {
    if (!CCNode::init()) return false;

    m_buttons = CCArray::create();
    auto winSize = CCDirector::get()->getWinSize();
    auto winBottom = CCDirector::get()->getScreenBottom();
    auto offset = ccp(winSize.width / 2 - 5.f, winBottom + editorUI->m_toolbarHeight - 6.f);
    auto rows = GameManager::get()->getIntGameVariable("0050");
    auto cols = GameManager::get()->getIntGameVariable("0049");

    auto giftButton = this->addButton(
        "GiftIcon.png"_spr, "GiftButton.png"_spr, "gift-button"_spr,
        [](auto sender) {
            SupportPopup::create()->show();
        }
    );
    if (!Mod::get()->getSavedValue<bool>("allium-gift-popup-shown", false)) {
        auto giftGlow = CCSprite::create("ButtonGlow.png"_spr);
        giftGlow->setColor(ccc3(255, 255, 0));
        giftGlow->setScale(0.95f);
        giftGlow->setZOrder(-1);
        giftGlow->setOpacity(150);
        giftGlow->setBlendFunc({GL_ONE, GL_ONE});
        giftGlow->runAction(CCRepeatForever::create(
            CCSequence::create(CCFadeTo::create(1.5f, 50), CCFadeTo::create(1.5f, 150), nullptr)
        ));
        giftButton->addChildAtPosition(giftGlow, Anchor::Center, ccp(0, 0));
    }
    
    auto panToggle = this->addDefaultToggle(
        "PanIcon.png"_spr, "pan-toggle"_spr, 
        [](auto sender) {
            BrushManager::get()->m_panEditorInBrush = !BrushManager::get()->m_panEditorInBrush;
        }
    );
    BrushManager::get()->m_panEditorInBrush = false;


    m_lineToggle = this->addDefaultToggle(
        "LineIcon.png"_spr, "line-toggle"_spr,
        [=, this](auto sender) {
            this->resetToggles(sender);
            if (sender->isToggled()) return;
            m_brushDrawer = LineBrushDrawer::create();
            LevelEditorLayer::get()->m_objectLayer->addChild(m_brushDrawer);
            m_brushDrawer->setID("brush-drawer"_spr);
        }
    );

    m_curveToggle = this->addDefaultToggle(
        "BezierIcon.png"_spr, "curve-toggle"_spr,
        [=, this](auto sender) {
            this->resetToggles(sender);
            if (sender->isToggled()) return;
            m_brushDrawer = CurveBrushDrawer::create();
            LevelEditorLayer::get()->m_objectLayer->addChild(m_brushDrawer);
            m_brushDrawer->setID("brush-drawer"_spr);
        }
    );

    m_freeToggle = this->addDefaultToggle(
        "BrushIcon.png"_spr, "free-toggle"_spr,
        [=, this](auto sender) {
            this->resetToggles(sender);
            if (sender->isToggled()) return;
            m_brushDrawer = FreeBrushDrawer::create();
            LevelEditorLayer::get()->m_objectLayer->addChild(m_brushDrawer);
            m_brushDrawer->setID("brush-drawer"_spr);
        }
    );

    // auto thicknessButton = this->addDefaultButton(
    //     "ThicknessIcon.png"_spr, "thickness-button"_spr,
    //     [](auto sender) {
    //         geode::openSettingsPopup(Mod::get());
    //     }
    // );

    // auto colorButton = this->addDefaultButton(
    //     "PaletteIcon.png"_spr, "color-button"_spr,
    //     [](auto sender) {
    //         geode::openSettingsPopup(Mod::get());
    //     }
    // );

    auto settingButton = this->addDefaultButton(
        "SettingIcon.png"_spr, "setting-button"_spr,
        [](auto sender) {
            geode::openSettingsPopup(Mod::get());
        }
    );

    auto finalizeButton = this->addDefaultButton(
        "FinalizeIcon.png"_spr, "finalize-button"_spr,
        [this](auto sender) {
            if (m_brushDrawer) {
                m_brushDrawer->clearOverlay();
                m_brushDrawer->updateLine();
            }
        }
    );

    m_buttonBar = EditButtonBar::create(m_buttons, offset, 0, false, cols, rows);
    m_buttonBar->setID("allium-tab-bar"_spr);

    return true;
}

void AlliumButtonBar::resetToggles(CCObject* sender) {
    if (sender != m_lineToggle) m_lineToggle->toggle(false);
    if (sender != m_curveToggle) m_curveToggle->toggle(false);
    if (sender != m_freeToggle) m_freeToggle->toggle(false);
    if (m_brushDrawer) {
        m_brushDrawer->clearOverlay();
        m_brushDrawer->updateLine();
        m_brushDrawer->removeFromParent();
        m_brushDrawer = nullptr;
    }
}

EditButtonBar* AlliumButtonBar::getButtonBar() const {
    return m_buttonBar;
}

BrushDrawer* AlliumButtonBar::getBrushDrawer() const {
    return m_brushDrawer;
}

CCMenuItemSpriteExtra* AlliumButtonBar::addDefaultButton(
    std::string_view spriteName, std::string_view id, 
    std::function<void(CCMenuItemSpriteExtra*)> const& callback
) {
    return this->addButton(spriteName, "NeutralButton.png"_spr, id, callback);
}

CCMenuItemSpriteExtra* AlliumButtonBar::addButton(
    std::string_view spriteName, std::string_view bgName, std::string_view id, 
    std::function<void(CCMenuItemSpriteExtra*)> const& callback
) {
    auto sprite = CCSprite::createWithSpriteFrameName(spriteName.data());
    auto bg = CCSprite::create(bgName.data());
    bg->addChildAtPosition(sprite, Anchor::Center, ccp(0, 0));

    auto button = CCMenuItemExt::createSpriteExtra(
        bg,
        [=](CCObject* sender) {
            callback(static_cast<CCMenuItemSpriteExtra*>(sender));
        }
    );
    button->setID(id.data());
    m_buttons->addObject(button);
    return button;
}

CCMenuItemToggler* AlliumButtonBar::addDefaultToggle(
    std::string_view spriteName, std::string_view id, 
    std::function<void(CCMenuItemToggler*)> const& callback
) {
    return addToggle(spriteName, "DeactiveButton.png"_spr, "ActiveButton.png"_spr, id, callback);
}

CCMenuItemToggler* AlliumButtonBar::addToggle(
    std::string_view spriteName, std::string_view bgOnName, std::string_view bgOffName, std::string_view id, 
    std::function<void(CCMenuItemToggler*)> const& callback
) {
    auto sprite = CCSprite::createWithSpriteFrameName(spriteName.data());
    auto bgOff = CCSprite::create(bgOffName.data());
    bgOff->addChildAtPosition(sprite, Anchor::Center, ccp(0, 0));

    sprite = CCSprite::createWithSpriteFrameName(spriteName.data());
    auto bgOn = CCSprite::create(bgOnName.data());
    bgOn->addChildAtPosition(sprite, Anchor::Center, ccp(0, 0));

    auto button = CCMenuItemExt::createToggler(
        bgOn,
        bgOff,
        [=](CCObject* sender) {
            callback(static_cast<CCMenuItemToggler*>(sender));
        }
    );
    button->setID(id.data());
    m_buttons->addObject(button);
    return button;
}