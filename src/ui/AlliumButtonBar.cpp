#include <ui/AlliumButtonBar.hpp>
#include <manager/BrushManager.hpp>
#include <util/BrushDrawer.hpp>
#include <util/CurveBrushDrawer.hpp>
#include <util/FreeBrushDrawer.hpp>
#include <util/LineBrushDrawer.hpp>
#include <util/PolygonBrushDrawer.hpp>
#include <util/TextBrushDrawer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <ui/SupportPopup.hpp>
#include <Geode/loader/Dispatch.hpp>
#include <Geode/loader/Event.hpp>

using namespace allium;
using namespace geode::prelude;

// static std::u32string lastImeInput = U"離れ離れの街を\n繋ぐ列車は行ってしまったね\n失くした言葉を知らないなら\nポケットで握りしめて";
// static std::u32string lastImeInput = U"You were tough, unforgiving\nMade me cry, all the time\nYou were mean, such an asshole\nSo, I had to say goodbye";
// static std::u32string lastImeInput = U"離れ離れの街を\n繋ぐ列車は行ってしまったね\n失くした言葉を知らないなら\nポケットで握りしめて\n\nあがいた息を捨てて\n延びる今日は眠って誤魔化せ\n失くした言葉を知らないなら\n各駅停車に乗り込んで\n\n夕方と退屈の\nお誘いを断って\n一人きり路地裏は\n決して急がないで\n\nほら横断歩道も\n待ってくれと言ってる\n見張る街角が\nあなたを引き留めてく";
// static std::u32string lastImeInput = U"It's\nalso\nblendable!\n\nTransparent\ntoo!\n\nOptimized\nfor\nobject\ncount!\n\nAllium\nupdate\nsoon!\n\nAlk1m123\nAlk1m123\nAlk1m123\nAlk1m123\n";
// auto listener = new EventListener(+[](std::wstring value) {
//     lastImeInput = string::utf8ToUtf32(string::wideToUtf8(value)).unwrapOrDefault();
//     return ListenerResult::Propagate;
// }, DispatchFilter<std::wstring>("alk.ime-input/win-result"));

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

    m_polygonToggle = this->addDefaultToggle(
        "PolygonIcon.png"_spr, "polygon-toggle"_spr,
        [=, this](auto sender) {
            this->resetToggles(sender);
            if (sender->isToggled()) return;
            m_brushDrawer = PolygonBrushDrawer::create();
            LevelEditorLayer::get()->m_objectLayer->addChild(m_brushDrawer);
            m_brushDrawer->setID("brush-drawer"_spr);
        }
    );

    m_textToggle = this->addDefaultToggle(
        "TextIcon.png"_spr, "text-toggle"_spr,
        [=, this](auto sender) {
            this->resetToggles(sender);
            if (sender->isToggled()) return;
            m_brushDrawer = TextBrushDrawer::create();
            LevelEditorLayer::get()->m_objectLayer->addChild(m_brushDrawer);
            m_brushDrawer->setID("brush-drawer"_spr);
        }
    );

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
    if (sender != m_polygonToggle) m_polygonToggle->toggle(false);
    if (sender != m_textToggle) m_textToggle->toggle(false);
    if (m_brushDrawer) {
        m_brushDrawer->clearOverlay();
        m_brushDrawer->updateLine();
        m_brushDrawer->removeFromParent();
        m_brushDrawer = nullptr;
    }
    EditorUI::get()->deselectAll();
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

MenuItemTogglerExtra* AlliumButtonBar::addDefaultToggle(
    std::string_view spriteName, std::string_view id,
    std::function<void(MenuItemTogglerExtra*)> const& callback
) {
    return this->addToggle(spriteName, "DeactiveButton.png"_spr, "ActiveButton.png"_spr, id, callback);
}

MenuItemTogglerExtra* AlliumButtonBar::addToggle(
    std::string_view spriteName, std::string_view bgOnName, std::string_view bgOffName, std::string_view id, 
    std::function<void(MenuItemTogglerExtra*)> const& callback
) {
    auto sprite = CCSprite::createWithSpriteFrameName(spriteName.data());
    auto bgOff = CCSprite::create(bgOffName.data());
    bgOff->addChildAtPosition(sprite, Anchor::Center, ccp(0, 0));

    sprite = CCSprite::createWithSpriteFrameName(spriteName.data());
    auto bgOn = CCSprite::create(bgOnName.data());
    bgOn->addChildAtPosition(sprite, Anchor::Center, ccp(0, 0));

    auto button = MenuItemTogglerExtra::create(
        bgOn,
        bgOff,
        callback
    );
    button->setID(id.data());
    m_buttons->addObject(button);
    return button;
}