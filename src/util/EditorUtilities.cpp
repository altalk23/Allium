#include <util/EditorUtilities.hpp>

using namespace geode::prelude;
using namespace allium;

EditorUtilities* EditorUtilities::get() {
    static EditorUtilities* instance = nullptr;
    if (!instance) {
        instance = new EditorUtilities();
    }
    return instance;
}

void EditorUtilities::setColor(int colorID) {
    m_colorID = colorID;
}

void EditorUtilities::addRectangle(cocos2d::CCPoint const& p1, cocos2d::CCPoint const& p2, cocos2d::CCPoint const& p3, cocos2d::CCPoint const& p4) {
    static constexpr int SQUARE_OBJECT_ID = 211;
    static constexpr float SQUARE_OBJECT_SCALE = 30.f;

    auto const center = (p1 + p3) / 2.0f;
    auto const angle = std::atan2(p2.y - p1.y, p2.x - p1.x);

    auto scaleX = p1.getDistance(p2) / SQUARE_OBJECT_SCALE;
    auto scaleY = p1.getDistance(p4) / SQUARE_OBJECT_SCALE;

    if (scaleX < 0.001f || scaleY < 0.001f) return; // Prevent scale reset

    auto object = LevelEditorLayer::get()->createObject(SQUARE_OBJECT_ID, center, false);
    object->setRotation(-angle * 180.0f / M_PI);

    object->updateCustomScaleX(scaleX);
    object->updateCustomScaleY(scaleY);

    if (object->m_baseColor) {
        object->m_baseColor->m_colorID = m_colorID;
        object->m_shouldUpdateColorSprite = true;
    }
    if (object->m_detailColor) {
        object->m_detailColor->m_colorID = m_colorID;
        object->m_shouldUpdateColorSprite = true;
    }

    LevelEditorLayer::get()->m_undoObjects->removeLastObject();
    m_objects.push_back(object);
}

void EditorUtilities::addCircle(cocos2d::CCPoint const& center, float radius) {
    static constexpr int CIRCLE_OBJECT_ID = 725;
    static constexpr float CIRCLE_OBJECT_SCALE = 9.f;

    auto object = LevelEditorLayer::get()->createObject(CIRCLE_OBJECT_ID, center, false);
    
    auto scaleX = radius * 2 / CIRCLE_OBJECT_SCALE;
    auto scaleY = radius * 2 / CIRCLE_OBJECT_SCALE;

    object->updateCustomScaleX(scaleX);
    object->updateCustomScaleY(scaleY);

    if (object->m_baseColor) {
        object->m_baseColor->m_colorID = m_colorID;
        object->m_shouldUpdateColorSprite = true;
    }
    if (object->m_detailColor) {
        object->m_detailColor->m_colorID = m_colorID;
        object->m_shouldUpdateColorSprite = true;
    }

    LevelEditorLayer::get()->m_undoObjects->removeLastObject();
    m_objects.push_back(object);
}

void EditorUtilities::finish() {
    auto objects = CCArray::create();
    for (auto const& object : m_objects) {
        objects->addObject(object);
    }
    LevelEditorLayer::get()->m_undoObjects->addObject(
        UndoObject::createWithArray(objects, UndoCommand::Paste)
    );
    m_objects.clear();
}