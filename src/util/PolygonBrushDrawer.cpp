#include <util/PolygonBrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <util/converter/TriangulatorConverter.hpp>

using namespace geode::prelude;
using namespace allium;

PolygonBrushDrawer* PolygonBrushDrawer::create() {
    auto ret = new (std::nothrow) PolygonBrushDrawer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool PolygonBrushDrawer::init() {
    if (!BrushDrawer::init()) return false;

    return true;
}

bool PolygonBrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    auto point2 = point;
    if (CCKeyboardDispatcher::get()->getShiftKeyPressed() && m_points.size()) point2 = BaseConverter::align(m_points.back(), point2);
    if (CCKeyboardDispatcher::get()->getAltKeyPressed()) point2 = BaseConverter::gridAlign(point2, 30.f);
    m_points.emplace_back(point2);
    if (m_points.size() >= 3) m_canUpdateLine = true;
    return true;
}
void PolygonBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    if (m_points.size() > 0) {
        auto point2 = point;
        if (CCKeyboardDispatcher::get()->getShiftKeyPressed() && m_points.size() > 1) point2 = BaseConverter::align(m_points[m_points.size() - 2], point2);
        else if (CCKeyboardDispatcher::get()->getAltKeyPressed()) point2 = BaseConverter::gridAlign(point2, 30.f);
        m_points.back() = point2;
    }
    this->updateOverlay();
}
void PolygonBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    if (m_points.size() > 0) {
        auto point2 = point;
        if (CCKeyboardDispatcher::get()->getShiftKeyPressed() && m_points.size() > 1) point2 = BaseConverter::align(m_points[m_points.size() - 2], point2);
        else if (CCKeyboardDispatcher::get()->getAltKeyPressed()) point2 = BaseConverter::gridAlign(point2, 30.f);
        m_points.back() = point2;
    }
    this->updateOverlay();
}

std::unique_ptr<BaseConverter> PolygonBrushDrawer::initializeConverter() {
    std::vector<TriangulatorConverter::Polygon> polygons;
    polygons.emplace_back();
    polygons.back().push_back(m_points);
    return std::make_unique<TriangulatorConverter>(
        std::move(polygons), false
    );
}

void PolygonBrushDrawer::updateOverlay() {
    this->clearOverlay();
    
    if (m_points.size() >= 3) BrushDrawer::updateOverlay();
    if (m_points.size() > 0) {
        auto lastPoint = m_points.back();
        for (auto point : m_points) {
            m_overlay->drawSegment(lastPoint, point, .5f, ccc4FFromccc3B(ccc3(255, 255, 191)));
            m_overlay->drawDot(lastPoint, 3.f, ccc4FFromccc3B(ccc3(255, 127, 127)));
            lastPoint = point;
        }
    }
}

void PolygonBrushDrawer::updateLine() {
    if (!m_canUpdateLine) return;
    m_canUpdateLine = false;
    
    BrushDrawer::updateLine();

    m_points.clear();
}