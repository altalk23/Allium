#include <util/LineBrushDrawer.hpp>

using namespace geode::prelude;
using namespace allium;

LineBrushDrawer* LineBrushDrawer::create() {
    auto ret = new (std::nothrow) LineBrushDrawer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LineBrushDrawer::init() {
    if (!BrushDrawer::init()) return false;

    return true;
}

bool LineBrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    m_firstPoint = point;
    m_lastPoint = point;
    this->updateOverlay();
    return true;
}
void LineBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    m_lastPoint = point;
    this->updateOverlay();
}
void LineBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    m_lastPoint = point;
    this->clearOverlay();
    this->updateLine();
}

void LineBrushDrawer::updateOverlay() {
    this->clearOverlay();
    m_overlay->drawSegment(m_firstPoint, m_lastPoint, m_lineWidth / 2.f, ccc4FFromccc3B(m_lineColor));
}

void LineBrushDrawer::updateLine() {
    static constexpr int SQUARE_OBJECT_ID = 211;
    static constexpr int WHITE_COLOR_ID = 1011;

    auto center = (m_firstPoint + m_lastPoint) / 2.0f;

    auto angle = std::atan2(m_lastPoint.y - m_firstPoint.y, m_lastPoint.x - m_firstPoint.x);

    auto offsetFirst = m_firstPoint - ccp(m_lineWidth / 2.f, 0).rotateByAngle(ccp(0, 0), angle);
    auto offsetLast = m_lastPoint + ccp(m_lineWidth / 2.f, 0).rotateByAngle(ccp(0, 0), angle);

    auto object = LevelEditorLayer::get()->createObject(SQUARE_OBJECT_ID, center, false);
    object->setRotation(-angle * 180.0f / M_PI);

    auto scaleX = offsetFirst.getDistance(offsetLast) / 30.f;
    auto scaleY = m_lineWidth / 30.f;

    object->updateCustomScaleX(scaleX);
    object->updateCustomScaleY(scaleY);

    if (object->m_baseColor) {
        object->m_baseColor->m_colorID = WHITE_COLOR_ID;
        object->m_shouldUpdateColorSprite = true;
    }
    if (object->m_detailColor) {
        object->m_detailColor->m_colorID = WHITE_COLOR_ID;
        object->m_shouldUpdateColorSprite = true;
    }
}