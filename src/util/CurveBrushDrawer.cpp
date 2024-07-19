#include <util/CurveBrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <agg-2.6/agg_curves.h>

using namespace geode::prelude;
using namespace allium;

CurveBrushDrawer* CurveBrushDrawer::create() {
    auto ret = new (std::nothrow) CurveBrushDrawer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CurveBrushDrawer::init() {
    if (!BrushDrawer::init()) return false;

    return true;
}

bool CurveBrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    if (m_points.size() == 4) {
        auto const point2Mirror = m_points[3] + (m_points[3] - m_points[2]);
        m_points[0] = m_points[3];
        m_points[1] = point2Mirror;
        m_points.erase(m_points.begin() + 2, m_points.end());
    }
    m_points.emplace_back(point);
    m_points.emplace_back(point);
    return true;
}
void CurveBrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {
    if (m_points.size() == 2) {
        m_points[1] = point;
    } else {
        m_canUpdateLine = true;
        auto const point2Mirror = m_points[3] + (m_points[3] - point);
        m_points[2] = point2Mirror;
        m_currentPoints = this->getGeneratedPoints();
    }
    this->updateOverlay();
}
void CurveBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    CurveBrushDrawer::handleTouchMove(point);
    this->updateOverlay();
    if (m_currentPoints.size() > 1) {
        m_previousPoints.insert(m_previousPoints.end(), m_currentPoints.begin(), m_currentPoints.end() - 1);
    }
    m_currentPoints.clear();
}

std::vector<std::array<double, 2>> CurveBrushDrawer::getGeneratedPoints() {
    std::vector<std::array<double, 2>> points;
    auto generator = agg::curve4_div();
    generator.approximation_scale(BrushManager::get()->getCurveRoughness());
    generator.init(
        m_points[0].x, m_points[0].y,
        m_points[1].x, m_points[1].y,
        m_points[2].x, m_points[2].y,
        m_points[3].x, m_points[3].y
    );
    double x, y;
    while (generator.vertex(&x, &y) != agg::path_cmd_stop) {
        points.push_back({x, y});
    }
    return points;
}

PolylineConverter CurveBrushDrawer::initializeConverter() {
    std::vector<PolylineConverter::Point> points;
    for (auto const& point : m_previousPoints) {
        points.push_back({point[0], point[1]});
    }
    for (auto const& point : m_currentPoints) {
        points.push_back({point[0], point[1]});
    }
    return PolylineConverter(BrushManager::get()->getLineWidth(), std::move(points));
}

void CurveBrushDrawer::updateOverlay() {
    this->clearOverlay();
    auto const point1Mirror = m_points[0] + (m_points[0] - m_points[1]);
    m_overlay->drawSegment(point1Mirror, m_points[1], .5f, ccc4FFromccc3B(ccc3(255, 255, 191)));
    m_overlay->drawDot(m_points[1], 3.f, ccc4FFromccc3B(ccc3(255, 127, 127)));
    m_overlay->drawDot(point1Mirror, 3.f, ccc4FFromccc3B(ccc3(127, 255, 127)));
    m_overlay->drawDot(m_points[0], 3.f, ccc4FFromccc3B(ccc3(127, 127, 255)));
    if (m_points.size() == 4) {
        auto const point2Mirror = m_points[3] + (m_points[3] - m_points[2]);
        m_overlay->drawSegment(point2Mirror, m_points[2], .5f, ccc4FFromccc3B(ccc3(255, 255, 191)));
        m_overlay->drawDot(m_points[2], 3.f, ccc4FFromccc3B(ccc3(255, 127, 127)));
        m_overlay->drawDot(point2Mirror, 3.f, ccc4FFromccc3B(ccc3(127, 255, 127)));
        m_overlay->drawDot(m_points[3], 3.f, ccc4FFromccc3B(ccc3(127, 127, 255)));
       
        BrushDrawer::updateOverlay();
    }   
}

void CurveBrushDrawer::updateLine() {
    if (!m_canUpdateLine) {
        m_points.clear();
        return;
    }
    m_canUpdateLine = false;

    BrushDrawer::updateLine();

    m_previousPoints.clear();
    m_currentPoints.clear();
    m_points.clear();
}