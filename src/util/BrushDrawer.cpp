#include <util/BrushDrawer.hpp>
#include <manager/BrushManager.hpp>

using namespace geode::prelude;
using namespace allium;

bool BrushDrawer::init() {
    m_overlay = DrawNodeExtension::create();
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

    for (auto const& objectList : convertedObjects) {
        for (auto const& object : objectList) {
            (void)object->drawIntoDrawNode(m_overlay, lineColor);
        }
    }
}
void BrushDrawer::clearOverlay() {
    m_overlay->clear();
}
bool BrushDrawer::isOverlayVisible() const {
    return m_overlay->m_nBufferCount > 0;
}

void BrushDrawer::updateLine() {
    m_canUpdateLine = false;

    auto convertedObjects = this->initializeConverter()->handleExtension();

    auto const lineColorID = BrushManager::get()->getColorID();

    auto objects = CCArray::create();

    auto editorLayer = LevelEditorLayer::get();
    // auto lastEditorId = editorLayer->m_lastUsedLinkedID;

    // if (!editorLayer->m_linkedGroupDict) editorLayer->m_linkedGroupDict = CCDictionary::create();

    // log::debug("Adding {} object groups", convertedObjects.size());
    for (auto const& objectList : convertedObjects) {
        auto currentArray = CCArray::create();
        // log::debug("Processing {} objects in group", objectList.size());
        for (auto const& object : objectList) {
            if (GEODE_UNWRAP_IF_OK(obj, object->addAsGameObject(LevelEditorLayer::get(), lineColorID))) {
                currentArray->addObject(obj);
                objects->addObject(obj);
            }
        }

        EditorUI::get()->deselectAll();
        EditorUI::get()->selectObjects(currentArray, false);
        EditorUI::get()->onGroupSticky(nullptr);
        EditorUI::get()->deselectAll();
    }

    LevelEditorLayer::get()->m_undoObjects->addObject(
        UndoObject::createWithArray(objects, UndoCommand::Paste)
    );
}
