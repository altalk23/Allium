#include <Geode/Geode.hpp>
#include <manager/BrushManager.hpp>
#include <ui/AlliumButtonBar.hpp>
#include <util/BrushDrawer.hpp>
#include <alphalaneous.editortab_api/API.hpp>

using namespace geode::prelude;
using namespace allium;

$on_mod(Loaded) {
    listenForKeybindSettingPresses("pan-editor-in-brush", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        BrushManager::get()->m_tempPanEditorInBrush = down;
    });
}

#include <Geode/modify/EditorUI.hpp>

struct EditorUIHook : Modify<EditorUIHook, EditorUI> {
    struct Fields {
        geode::Ref<AlliumButtonBar> m_buttonBar;
        bool m_outsideDeadzone = false;
    };

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
        auto tabIcon = CCSprite::createWithSpriteFrameName("EditorIcon.png"_spr);
        tabIcon->setScale(0.2f);
        auto tab = alpha::editor_tabs::getMode(alpha::editor_tabs::Build)->createTab(
            "alk.allium"_spr, m_fields->m_buttonBar->getEditorTab(), tabIcon, 0
        );
        m_fields->m_buttonBar->getEditorTab()->addEventListener(
            alpha::editor_tabs::Tab::SwitchTabEvent(tab),
            [this](bool visible) {
                if (!visible) m_fields->m_buttonBar->resetToggles(nullptr);
            }
        );

        return true;
    }

    CCPoint getLayerPosition(CCTouch* touch) {
        auto objectLayer = LevelEditorLayer::get()->m_objectLayer;
        auto glPoint = CCDirector::get()->convertToGL(
            m_fields->m_outsideDeadzone ? touch->getLocationInView() : touch->getStartLocationInView()
        );
		return objectLayer->convertToNodeSpace(this->convertToWorldSpace(glPoint));
	}

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->panEditorInBrush() && m_fields->m_buttonBar->getBrushDrawer()) {
            auto layerPosition = this->getLayerPosition(touch);
            m_fields->m_outsideDeadzone = !m_fields->m_buttonBar->getBrushDrawer()->usesDeadzone();
            m_fields->m_buttonBar->getBrushDrawer()->handleTouchStart(layerPosition);

            return true;
        }
        return EditorUI::ccTouchBegan(touch, event);
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->panEditorInBrush() && m_fields->m_buttonBar->getBrushDrawer()) {
            if (!m_fields->m_outsideDeadzone) {
                auto deadzone = Mod::get()->getSettingValue<double>("deadzone-radius");
                m_fields->m_outsideDeadzone = ccpDistance(touch->getStartLocation(), touch->getLocation()) > deadzone;
            }
            if (m_fields->m_outsideDeadzone) {
                auto layerPosition = this->getLayerPosition(touch);
                m_fields->m_buttonBar->getBrushDrawer()->handleTouchMove(layerPosition);
            }

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
  
  $override
    void updateZoom(float zoom) {
        EditorUI::updateZoom(zoom);
        if (auto drawer = m_fields->m_buttonBar->getBrushDrawer()) {
            if (drawer->isOverlayVisible()) {
                drawer->updateOverlay();
            }
        }
    }
};
