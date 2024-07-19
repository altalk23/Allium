#include <util/LineBrushDrawer.hpp>
#include <manager/BrushManager.hpp>

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
    return true;
}
void LineBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    m_canUpdateLine = true;
    m_lastPoint = point;
    this->updateOverlay();
}
void LineBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    m_lastPoint = point;
    this->clearOverlay();
    this->updateLine();
}

PolylineConverter LineBrushDrawer::initializeConverter() {
    std::vector<PolylineConverter::Point> points;
    points.push_back({m_firstPoint.x, m_firstPoint.y});
    points.push_back({m_lastPoint.x, m_lastPoint.y});
    return PolylineConverter(BrushManager::get()->getLineWidth(), std::move(points));
}

void LineBrushDrawer::updateOverlay() {
    this->clearOverlay();
    
    BrushDrawer::updateOverlay();

}

void LineBrushDrawer::updateLine() {
    if (!m_canUpdateLine) return;
    m_canUpdateLine = false;
    
    BrushDrawer::updateLine();
}