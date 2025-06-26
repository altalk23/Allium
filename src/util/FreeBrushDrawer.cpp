#include <util/FreeBrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <util/converter/PolylineConverter.hpp>

using namespace geode::prelude;
using namespace allium;

FreeBrushDrawer* FreeBrushDrawer::create() {
    auto ret = new (std::nothrow) FreeBrushDrawer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FreeBrushDrawer::init() {
    if (!BrushDrawer::init()) return false;

    return true;
}

bool FreeBrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    m_points.clear();
    m_points.emplace_back(point);
    return true;
}
void FreeBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    m_points.emplace_back(point);
    m_points = BaseConverter::simplify(m_points, BrushManager::get()->getFreeThreshold());
    if (m_points.size() >= 2) m_canUpdateLine = true;
    this->updateOverlay();
}
void FreeBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    m_points.emplace_back(point);
    m_points = BaseConverter::simplify(m_points, BrushManager::get()->getFreeThreshold());
    if (m_points.size() >= 2) m_canUpdateLine = true;
    this->clearOverlay();
    this->updateLine();
}

std::unique_ptr<BaseConverter> FreeBrushDrawer::initializeConverter() {
    std::vector<Point> points;
    points.insert(points.end(), m_points.begin(), m_points.end());
    return std::make_unique<PolylineConverter>(
        BrushManager::get()->getLineWidth(), std::move(points)
    );
}

void FreeBrushDrawer::updateOverlay() {
    this->clearOverlay();
    if (m_points.size() < 2) return;

    BrushDrawer::updateOverlay();
}

void FreeBrushDrawer::updateLine() {
    if(!m_canUpdateLine) return;
    m_canUpdateLine = false;

    BrushDrawer::updateLine();

    m_points.clear();
}