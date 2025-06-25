#include <data/Geometry.hpp>
#include <util/DrawNodeExtension.hpp>

using namespace geode::prelude;
using namespace allium;

geode::Result<GameObject*> Rect::addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const {
    static constexpr int SQUARE_OBJECT_ID = 211;
    static constexpr float SQUARE_OBJECT_SCALE = 30.f;

    auto const center = (p1 + p3) / 2.0f;
    auto const angle = std::atan2(p2.y - p1.y, p2.x - p1.x);

    auto scaleX = p1.getDistance(p2) / SQUARE_OBJECT_SCALE;
    auto scaleY = p1.getDistance(p4) / SQUARE_OBJECT_SCALE;

    if (scaleX < 0.001f || scaleY < 0.001f) return Err("Scale too small"); // Prevent scale reset

    auto object = editorLayer->createObject(SQUARE_OBJECT_ID, center, false);
    object->setRotation(-angle * 180.0f / M_PI);

    object->updateCustomScaleX(scaleX);
    object->updateCustomScaleY(scaleY);

    if (object->m_baseColor) {
        object->m_baseColor->m_colorID = colorID;
        object->m_shouldUpdateColorSprite = true;
    }
    if (object->m_detailColor) {
        object->m_detailColor->m_colorID = colorID;
        object->m_shouldUpdateColorSprite = true;
    }

    editorLayer->m_undoObjects->removeLastObject();
    return Ok(object);
}

geode::Result<> Rect::drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const {
    node->drawPolygon(
        std::array<cocos2d::CCPoint, 4>{
            {p1, p2, p3, p4}
        }.data(), 4,
        ccc4FFromccc3B(color)
    );

    return Ok();
}

geode::Result<GameObject*> Circle::addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const {
    static constexpr int CIRCLE_OBJECT_ID = 725;
    static constexpr float CIRCLE_OBJECT_SCALE = 9.f;

    auto object = editorLayer->createObject(CIRCLE_OBJECT_ID, center, false);
    
    auto scaleX = radius * 2 / CIRCLE_OBJECT_SCALE;
    auto scaleY = radius * 2 / CIRCLE_OBJECT_SCALE;

    object->updateCustomScaleX(scaleX);
    object->updateCustomScaleY(scaleY);

    if (object->m_baseColor) {
        object->m_baseColor->m_colorID = colorID;
        object->m_shouldUpdateColorSprite = true;
    }
    if (object->m_detailColor) {
        object->m_detailColor->m_colorID = colorID;
        object->m_shouldUpdateColorSprite = true;
    }

    editorLayer->m_undoObjects->removeLastObject();
    return Ok(object);
}

geode::Result<> Circle::drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const {
    std::array<cocos2d::CCPoint, 32> points;
    for (size_t i = 0; i < 32; ++i) {
        auto const angle = i * 2 * M_PI / 32;
        points[i] = CCPoint::forAngle(angle) * radius + center;
    }
    node->drawPolygon(
        points.data(), points.size(),
        ccc4FFromccc3B(color)
    );

    return Ok();
}

geode::Result<GameObject*> Triangle::addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const {
    // The game object we have has points at (0, 0), (1, 0) and (1, 1)
    // We need to find the affine transform that transforms this object into our triangle
    static constexpr int TRIANGLE_OBJECT_ID = 693;
    static constexpr float TRIANGLE_OBJECT_SCALE = 30.f;

    // Thank you flowvix!
    auto const p1 = this->p1 / TRIANGLE_OBJECT_SCALE;
    auto const p2 = this->p2 / TRIANGLE_OBJECT_SCALE;
    auto const p3 = this->p3 / TRIANGLE_OBJECT_SCALE;
    auto const pos = (p3 + p2) / 2.0;
    auto const mat = Mat2(p2-p1, p3-p1) * Mat2::fromAngle(M_PI * -0.5);
    Point const i = mat.data[0];
    Point const j = mat.data[1];
    auto const iAngle = i.angleTo({1.0, 0.0});
    auto const jAngle = j.angleTo({0.0, 1.0});
    auto const iLen = i.getLength();
    auto const jLen = j.getLength();

    auto object = editorLayer->createObject(TRIANGLE_OBJECT_ID, pos * 30.0, false);

    object->setRotationX(jAngle * 180.0f / M_PI);
    object->setRotationY(iAngle * 180.0f / M_PI);
    object->updateCustomScaleX(iLen);
    object->updateCustomScaleY(jLen);

    if (object->m_baseColor) {
        object->m_baseColor->m_colorID = colorID;
        object->m_shouldUpdateColorSprite = true;
    }
    if (object->m_detailColor) {
        object->m_detailColor->m_colorID = colorID;
        object->m_shouldUpdateColorSprite = true;
    }

    editorLayer->m_undoObjects->removeLastObject();
    return Ok(object);
}

geode::Result<> Triangle::drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const {
    node->drawPolygon(
        std::array<cocos2d::CCPoint, 3>{
            {p1, p2, p3}
        }.data(), 3,
        ccc4FFromccc3B(color)
    );

    return Ok();
}