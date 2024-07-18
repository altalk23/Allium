#include <Geode/Geode.hpp>
#include <manager/BrushManager.hpp>
#include <ui/AlliumButtonBar.hpp>
#include <util/BrushDrawer.hpp>

#ifdef GEODE_IS_WINDOWS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;
using namespace allium;

#include <Geode/modify/EditorUI.hpp>

struct EditorUIHook : Modify<EditorUIHook, EditorUI> {
    struct Fields {
        geode::Ref<AlliumButtonBar> m_buttonBar;
        int tabTag = 0;
    };

    $override
    void toggleMode(CCObject* sender) {
        auto tag = sender->getTag();

        if (tag != 1 && m_fields->m_buttonBar) {
            m_fields->m_buttonBar->resetToggles(sender);
        }
        EditorUI::toggleMode(sender);
    }

    $override
    void onSelectBuildTab(CCObject* sender) {
        auto tag = sender->getTag();

        if (tag != m_fields->tabTag && m_fields->m_buttonBar) {
            m_fields->m_buttonBar->resetToggles(sender);
        }
        EditorUI::onSelectBuildTab(sender);
    }

    $override
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        if (!Mod::get()->getSavedValue<bool>("allium-build-tab-announcement-shown", false)) {
            auto popup = createQuickPopup(
                "Allium", "Allium is available in the build tab now! You can access it there.",
                "OK", nullptr, nullptr, false
            );
            popup->m_scene = m_editorLayer;
            popup->show();

            Mod::get()->setSavedValue("allium-build-tab-announcement-shown", true);
        }

        m_fields->m_buttonBar = AlliumButtonBar::create(this);

        m_fields->m_buttonBar->getButtonBar()->setZOrder(10);
        m_fields->m_buttonBar->getButtonBar()->setVisible(false);
        m_createButtonBars->addObject(m_fields->m_buttonBar->getButtonBar());

        auto spriteOn = CCSprite::createWithSpriteFrameName("EditorIcon.png"_spr);
        spriteOn->setScale(0.2f);
        auto onBg = CCSprite::createWithSpriteFrameName("GJ_tabOn_001.png");
        onBg->addChildAtPosition(spriteOn, Anchor::Center, ccp(0, 0));
        
        auto spriteOff = CCSprite::createWithSpriteFrameName("EditorIcon.png"_spr);
        spriteOff->setScale(0.2f);
        auto offBg = CCSprite::createWithSpriteFrameName("GJ_tabOff_001.png");
        offBg->addChildAtPosition(spriteOff, Anchor::Center, ccp(0, 0));
        offBg->setOpacity(150);

        m_fields->tabTag = m_tabsArray->count();

        auto tabToggle = CCMenuItemExt::createToggler(
            offBg, onBg, [this](CCObject* sender) {
                this->onSelectBuildTab(sender);
                static_cast<CCMenuItemToggler*>(sender)->toggle(false);
            }
        );
        tabToggle->setID("allium-tab-toggle"_spr);
        tabToggle->setTag(m_fields->tabTag);
        m_tabsArray->addObject(tabToggle);
        m_tabsMenu->addChild(tabToggle);
        m_tabsMenu->updateLayout();

        this->addChild(m_fields->m_buttonBar->getButtonBar());

    #ifdef GEODE_IS_WINDOWS

        // Adds the keybind listener for panning in brush mode
        using namespace keybinds;

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                BrushManager::get()->m_tempPanEditorInBrush = true;
            }
            else {
                BrushManager::get()->m_tempPanEditorInBrush = false;
            }
            return ListenerResult::Propagate;
        }, "pan-editor-in-brush"_spr);
    #endif

        return true;
    }

    CCPoint getLayerPosition(CCTouch* touch) {
        auto objectLayer = LevelEditorLayer::get()->m_objectLayer;
        auto glPoint = CCDirector::get()->convertToGL(touch->getLocationInView());
		return objectLayer->convertToNodeSpace(this->convertToWorldSpace(glPoint));
	}

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->panEditorInBrush() && m_fields->m_buttonBar->getBrushDrawer()) {
            auto layerPosition = this->getLayerPosition(touch);
            m_fields->m_buttonBar->getBrushDrawer()->handleTouchStart(layerPosition);

            return true;
        }
        return EditorUI::ccTouchBegan(touch, event);
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->panEditorInBrush() && m_fields->m_buttonBar->getBrushDrawer()) {
            auto layerPosition = this->getLayerPosition(touch);
            m_fields->m_buttonBar->getBrushDrawer()->handleTouchMove(layerPosition);

            return;
        }
        return EditorUI::ccTouchMoved(touch, event);
    }

    $override
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->panEditorInBrush() && m_fields->m_buttonBar->getBrushDrawer()) {
            auto layerPosition = this->getLayerPosition(touch);
            m_fields->m_buttonBar->getBrushDrawer()->handleTouchEnd(layerPosition);

            return;
        }
        return EditorUI::ccTouchEnded(touch, event);
    }
};

#ifdef GEODE_IS_WINDOWS

$execute {
    using namespace keybinds;

    BindManager::get()->registerBindable({
        "pan-editor-in-brush"_spr,
        "Pan Editor In Brush Mode",
        "Allows you to pan in the editor when you have brush enabled.",
        { Keybind::create(KEY_Space) },
        "Allium/Brushes"
    });
}

#endif