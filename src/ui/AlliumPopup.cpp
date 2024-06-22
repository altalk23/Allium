#include <Geode/Geode.hpp>
#include <ui/AlliumPopup.hpp>

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

void AlliumPopup::brushToggleCallback(CCMenuItemToggler* toggle) {
    if (toggle != m_noneBrushToggle) m_noneBrushToggle->toggle(false);
    if (toggle != m_lineBrushToggle) m_lineBrushToggle->toggle(false);
    if (toggle != m_curveBrushToggle) m_curveBrushToggle->toggle(false);
    if (toggle != m_freeBrushToggle) m_freeBrushToggle->toggle(false);
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

    m_noneBrushToggle->toggle(true);

    m_brushMenu->updateLayout();
    m_mainColumn->updateLayout();

    return true;
}