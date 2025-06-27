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

    auto editorLayer = LevelEditorLayer::get();
    auto lastId = -1;
    auto currentArray = CCArray::create();
    // auto lastEditorId = editorLayer->m_lastUsedLinkedID;

    // if (!editorLayer->m_linkedGroupDict) editorLayer->m_linkedGroupDict = CCDictionary::create();

    
    for (auto const& object : convertedObjects) {
        if (GEODE_UNWRAP_IF_OK(obj, object->addAsGameObject(LevelEditorLayer::get(), lineColorID))) {
            if (lastId != object->idx) {
                if (currentArray->count() > 0) {
                    EditorUI::get()->deselectAll();
                    EditorUI::get()->selectObjects(currentArray, false);
                    EditorUI::get()->onGroupSticky(nullptr);
                    currentArray = CCArray::create();
                }
                lastId = object->idx;
            }

            currentArray->addObject(obj);
            objects->addObject(obj);
        }
    }

    if (currentArray->count() > 0) {
        EditorUI::get()->deselectAll();
        EditorUI::get()->selectObjects(currentArray, false);
        EditorUI::get()->onGroupSticky(nullptr);
    }
    EditorUI::get()->deselectAll();

    LevelEditorLayer::get()->m_undoObjects->addObject(
        UndoObject::createWithArray(objects, UndoCommand::Paste)
    );
}
