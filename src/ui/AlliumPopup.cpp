#include <Geode/Geode.hpp>
#include <manager/BrushManager.hpp>
#include <ui/AlliumPopup.hpp>
#include <util/BrushDrawer.hpp>
#include <util/CurveBrushDrawer.hpp>
#include <util/LineBrushDrawer.hpp>

using namespace geode::prelude;
using namespace allium;

AlliumPopup* AlliumPopup::create() {
    auto ret = new (std::nothrow) AlliumPopup();
    if (ret && ret->initAnchored(400.f, 240.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCNode* AlliumPopup::getEnablePanningSprite() {
    auto enableSprite = BasedButtonSprite::create(
        CCLabelBMFont::create("Enable\nPanning", "bigFont.fnt"), BaseType::Editor, 
        static_cast<int>(EditorBaseSize::Normal), static_cast<int>(EditorBaseColor::Gray)
    );
    enableSprite->setTopRelativeScale(1.6f);
    return enableSprite;
}
CCNode* AlliumPopup::getDisablePanningSprite() {
    auto disableSprite = BasedButtonSprite::create(
        CCLabelBMFont::create("Disable\nPanning", "bigFont.fnt"), BaseType::Editor, 
        static_cast<int>(EditorBaseSize::Normal), static_cast<int>(EditorBaseColor::Green)
    );
    disableSprite->setTopRelativeScale(1.6f);
    return disableSprite;
}

void AlliumPopup::createPanButton() {
    auto buttonMenu = EditorUI::get()->getChildByID("editor-buttons-menu");
    auto myButton = CCMenuItemExt::createSpriteExtra(
        BrushManager::get()->m_panEditorInBrush ? this->getDisablePanningSprite() : this->getEnablePanningSprite(), 
        [this](CCObject* sender) {
            auto myButton = static_cast<CCMenuItemSprite*>(sender);
            if (BrushManager::get()->m_panEditorInBrush) {
                BrushManager::get()->m_panEditorInBrush = false;
                myButton->setNormalImage(this->getEnablePanningSprite());
            }
            else {
                BrushManager::get()->m_panEditorInBrush = true;
                myButton->setNormalImage(this->getDisablePanningSprite());
            }
            myButton->setContentSize({ 40.f, 40.f });
        }
    );
    myButton->setID("allium-panning-button"_spr);
    // size set in Node IDs itself
    myButton->setContentSize({ 40.f, 40.f });

    buttonMenu->addChild(myButton);
    buttonMenu->updateLayout();
}
void AlliumPopup::createFinalizeButton() {
    auto buttonMenu = EditorUI::get()->getChildByID("editor-buttons-menu");

    auto topSprite = CCLabelBMFont::create("Finalize", "bigFont.fnt");

    auto mySprite = BasedButtonSprite::create(
        topSprite, BaseType::Editor, 
        static_cast<int>(EditorBaseSize::Normal), static_cast<int>(EditorBaseColor::Orange)
    );
    mySprite->setTopRelativeScale(1.6f);

    auto myButton = CCMenuItemExt::createSpriteExtra(
        mySprite, [this](CCObject* sender) {
            if (BrushManager::get()->m_currentDrawer) {
                BrushManager::get()->m_currentDrawer->clearOverlay();
                BrushManager::get()->m_currentDrawer->updateLine();
            }
        }
    );
    myButton->setID("allium-finalize-button"_spr);
    // size set in Node IDs itself
    myButton->setContentSize({ 40.f, 40.f });

    buttonMenu->addChild(myButton);
    buttonMenu->updateLayout();
}

void AlliumPopup::brushToggleCallback(CCMenuItemToggler* toggle) {
    if (toggle != m_noneBrushToggle) m_noneBrushToggle->toggle(false);
    if (toggle != m_lineBrushToggle) m_lineBrushToggle->toggle(false);
    if (toggle != m_curveBrushToggle) m_curveBrushToggle->toggle(false);
    if (toggle != m_freeBrushToggle) m_freeBrushToggle->toggle(false);

    auto objectLayer = LevelEditorLayer::get()->m_objectLayer;
    auto brushDrawer = static_cast<BrushDrawer*>(objectLayer->getChildByID("brush-drawer"_spr));
    if (brushDrawer) {
        brushDrawer->removeFromParent();
        brushDrawer = nullptr;
    }
    auto buttonMenu = EditorUI::get()->getChildByID("editor-buttons-menu");
    auto finalizeButton = static_cast<CCMenuItem*>(buttonMenu->getChildByID("allium-finalize-button"_spr));
    if (finalizeButton) {
        finalizeButton->removeFromParent();
    }
    auto panningButton = static_cast<CCMenuItem*>(buttonMenu->getChildByID("allium-panning-button"_spr));
    if (panningButton) {
        panningButton->removeFromParent();
    }
    
    if (toggle == m_lineBrushToggle) {
        brushDrawer = LineBrushDrawer::create();

        BrushManager::get()->m_currentBrush = BrushType::Line;
        this->createPanButton();
    }
    else if (toggle == m_curveBrushToggle) {
        brushDrawer = CurveBrushDrawer::create();

        BrushManager::get()->m_currentBrush = BrushType::Curve;
        this->createPanButton();
        this->createFinalizeButton();
    }
    else {
        BrushManager::get()->m_currentBrush = BrushType::None;

        BrushManager::get()->m_panEditorInBrush = false;
    }
    if (brushDrawer) {
        brushDrawer->setID("brush-drawer"_spr);
        objectLayer->addChild(brushDrawer);
    }
    BrushManager::get()->m_currentDrawer = brushDrawer;

}

void AlliumPopup::createBrushToggle(std::string_view name, std::string const& id, CCMenuItemToggler*& toggle) {
    auto toggleAddress = &toggle;
    toggle = CCMenuItemExt::createTogglerWithStandardSprites(
        0.5f, [=, this] (CCObject* sender) {
            this->brushToggleCallback(*toggleAddress);
        }
    );
    toggle->toggle(false);
    toggle->setID(Mod::get()->expandSpriteName(id + "-toggle"_spr).data());
    m_brushMenu->addChild(toggle);

    auto label = CCLabelBMFont::create(name.data(), "bigFont.fnt");
    label->setScale(0.4f);
    label->setID(Mod::get()->expandSpriteName(id + "-label"_spr).data());
    m_brushMenu->addChild(label);
}

bool AlliumPopup::setup() {

    // Every menu is put into this column
    m_mainColumn = CCNode::create();
    m_mainColumn->setContentSize(m_mainLayer->getContentSize() - ccp(20.f, 20.f));
    m_mainColumn->setAnchorPoint(ccp(0.5f, 0.5f));
    m_mainColumn->setLayout(
        ColumnLayout::create()
            ->setGap(10.f)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoGrowAxis(0)
    );
    m_mainLayer->addChildAtPosition(m_mainColumn, Anchor::Center, ccp(0, 0));

    // Brush menu
    m_brushMenu = CCMenu::create();
    m_brushMenu->setLayout(
        RowLayout::create()
            ->setGap(5.f)
            ->setAutoScale(false)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoGrowAxis(0)
    );
    m_mainColumn->addChild(m_brushMenu);

    auto brushLabel = CCLabelBMFont::create("Brush type:", "goldFont.fnt");
    brushLabel->setScale(0.7f);
    m_brushMenu->addChild(brushLabel);

    createBrushToggle("None", "none-brush", m_noneBrushToggle);
    createBrushToggle("Line", "line-brush", m_lineBrushToggle);
    createBrushToggle("Curve", "curve-brush", m_curveBrushToggle);
    createBrushToggle("Free", "free-brush", m_freeBrushToggle);

    switch (BrushManager::get()->m_currentBrush) {
        case BrushType::None:
            m_noneBrushToggle->toggle(true);
            break;
        case BrushType::Line:
            m_lineBrushToggle->toggle(true);
            break;
        case BrushType::Curve:
            m_curveBrushToggle->toggle(true);
            break;
        case BrushType::Free:
            m_freeBrushToggle->toggle(true);
            break;
    }

    m_brushMenu->updateLayout();
    m_mainColumn->updateLayout();

    return true;
}