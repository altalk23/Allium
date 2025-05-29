#include <util/LineBrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <util/converter/PolylineConverter.hpp>

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

std::unique_ptr<BaseConverter> LineBrushDrawer::initializeConverter() {
    std::vector<Point> points;
    points.push_back(m_firstPoint);
    points.push_back(m_lastPoint);
    return std::make_unique<PolylineConverter>(
        BrushManager::get()->getLineWidth(), std::move(points)
    );
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