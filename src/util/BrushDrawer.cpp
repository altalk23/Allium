#include <util/BrushDrawer.hpp>
#include <manager/BrushManager.hpp>

using namespace geode::prelude;
using namespace allium;

bool BrushDrawer::init() {
    m_overlay = cocos2d::CCDrawNode::create();
    this->addChild(m_overlay);

    return true;
}

bool BrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    m_canUpdateLine = true;
    return false;
}
void BrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {}
void BrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {}

void BrushDrawer::updateOverlay() {
    auto convertedObjects = this->initializeConverter()->handleExtension();

    auto lineColor = BrushManager::get()->getColor();

    for (auto const& object : convertedObjects) {
        (void)object->drawIntoDrawNode(m_overlay, lineColor);
    }
}
void BrushDrawer::clearOverlay() {
    m_overlay->clear();
}

void BrushDrawer::updateLine() {
    m_canUpdateLine = false;

    auto convertedObjects = this->initializeConverter()->handleExtension();

    auto const lineColorID = BrushManager::get()->getColorID();

    auto objects = CCArray::create();

    for (auto const& object : convertedObjects) {
        if (GEODE_UNWRAP_IF_OK(obj, object->addAsGameObject(LevelEditorLayer::get(), lineColorID))) {
            objects->addObject(obj);
        }
    }

    LevelEditorLayer::get()->m_undoObjects->addObject(
        UndoObject::createWithArray(objects, UndoCommand::Paste)
    );
}
