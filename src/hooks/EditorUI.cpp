#include <Geode/Geode.hpp>
#include <manager/BrushManager.hpp>
#include <ui/AlliumButtonBar.hpp>
#include <util/BrushDrawer.hpp>
#include <alphalaneous.editortab_api/include/EditorTabs.hpp>

#ifdef GEODE_IS_WINDOWS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;
using namespace allium;

#include <Geode/modify/EditorUI.hpp>

struct EditorUIHook : Modify<EditorUIHook, EditorUI> {
    struct Fields {
        geode::Ref<AlliumButtonBar> m_buttonBar;
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

        EditorTabs::addTab(this, TabType::BUILD, "allium"_spr, [this](EditorUI* ui, CCMenuItemToggler* toggler) -> CCNode* { 
            auto sprite = CCSprite::createWithSpriteFrameName("EditorIcon.png"_spr);
            sprite->setScale(0.2f);
            EditorTabUtils::setTabIcon(toggler, sprite);

            return m_fields->m_buttonBar->getButtonBar();
        }, [this](EditorUI*, bool state, CCNode*) {
            if (!state) {
                m_fields->m_buttonBar->resetToggles(nullptr);
            }
        });


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