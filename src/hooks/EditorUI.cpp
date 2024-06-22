#include <Geode/Geode.hpp>
#include <ui/AlliumPopup.hpp>

using namespace geode::prelude;
using namespace allium;

#include <Geode/modify/EditorUI.hpp>

struct EditorUIHook : Modify<EditorUIHook, EditorUI> {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;


        auto buttonMenu = this->getChildByID("editor-buttons-menu");

        // temporary, will be changed with a custom sprite
        auto topSprite = CCLabelBMFont::create("Allium", "bigFont.fnt");

        auto mySprite = BasedButtonSprite::create(
            topSprite, BaseType::Editor, 
            static_cast<int>(EditorBaseSize::Normal), static_cast<int>(EditorBaseColor::LightBlue)
        );
        mySprite->setTopRelativeScale(1.6f);

        auto myButton = CCMenuItemExt::createSpriteExtra(
            mySprite, [this](CCObject* sender) {
                AlliumPopup::create()->show();
            }
        );
        myButton->setID("allium-button"_spr);
        // size set in Node IDs itself
        myButton->setContentSize({ 40.f, 40.f });

        buttonMenu->addChild(myButton);
        buttonMenu->updateLayout();

        return true;
    }
};