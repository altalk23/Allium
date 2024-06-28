#include <Geode/Geode.hpp>
#include <manager/BrushManager.hpp>
#include <ui/AlliumPopup.hpp>
#include <util/BrushDrawer.hpp>

#ifdef GEODE_IS_WINDOWS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;
using namespace allium;

#include <Geode/modify/EditorUI.hpp>

struct EditorUIHook : Modify<EditorUIHook, EditorUI> {
    $override
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        
        BrushManager::get()->m_currentDrawer = nullptr;
        BrushManager::get()->m_currentBrush = BrushType::None;
        BrushManager::get()->m_panEditorInBrush = false;

        auto buttonMenu = this->getChildByID("editor-buttons-menu");

        // temporary, will be changed with a custom sprite
        auto mySprite = BasedButtonSprite::create(
            CCLabelBMFont::create("Allium", "bigFont.fnt"), BaseType::Editor, 
            static_cast<int>(EditorBaseSize::Normal), static_cast<int>(EditorBaseColor::LightBlue)
        );
        mySprite->setTopRelativeScale(1.6f);

        auto myButton = CCMenuItemExt::createSpriteExtra(
            mySprite, [this](CCObject* sender) {
                if (BrushManager::get()->m_currentDrawer) {
                    BrushManager::get()->m_currentDrawer->clearOverlay();
                    BrushManager::get()->m_currentDrawer->updateLine();
                }
                AlliumPopup::create()->show();
            }
        );
        myButton->setID("allium-button"_spr);
        // size set in Node IDs itself
        myButton->setContentSize({ 40.f, 40.f });

        buttonMenu->addChild(myButton);
        buttonMenu->updateLayout();

    #ifdef GEODE_IS_WINDOWS

        // Adds the keybind listener for panning in brush mode
        using namespace keybinds;

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                BrushManager::get()->m_panEditorInBrush = true;
            }
            else {
                BrushManager::get()->m_panEditorInBrush = false;
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
        if (!BrushManager::get()->m_panEditorInBrush && BrushManager::get()->m_currentDrawer) {
            auto layerPosition = this->getLayerPosition(touch);
            BrushManager::get()->m_currentDrawer->handleTouchStart(layerPosition);

            return true;
        }
        return EditorUI::ccTouchBegan(touch, event);
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->m_panEditorInBrush && BrushManager::get()->m_currentDrawer) {
            auto layerPosition = this->getLayerPosition(touch);
            BrushManager::get()->m_currentDrawer->handleTouchMove(layerPosition);

            return;
        }
        return EditorUI::ccTouchMoved(touch, event);
    }

    $override
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        if (!BrushManager::get()->m_panEditorInBrush && BrushManager::get()->m_currentDrawer) {
            auto layerPosition = this->getLayerPosition(touch);
            BrushManager::get()->m_currentDrawer->handleTouchEnd(layerPosition);

            return;
        }
        return EditorUI::ccTouchEnded(touch, event);
    }

    $override
    void showUI(bool show) {
        EditorUI::showUI(show);

        auto alliumButton = static_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("allium-button"_spr));
        if (alliumButton) {
            alliumButton->setEnabled(show);
            alliumButton->setVisible(show);
        }

        auto panButton = static_cast<CCMenuItem*>(this->getChildByIDRecursive("allium-panning-button"_spr));
        if (panButton) {
            panButton->setEnabled(show);
            panButton->setVisible(show);
        }

        auto finalizeButton = static_cast<CCMenuItem*>(this->getChildByIDRecursive("allium-finalize-button"_spr));
        if (finalizeButton) {
            finalizeButton->setEnabled(show);
            finalizeButton->setVisible(show);
        }
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