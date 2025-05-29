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
    m_points = BaseConverter::simplify(m_points);
    if (m_points.size() >= 2) m_canUpdateLine = true;
    this->updateOverlay();
}
void FreeBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    m_points.emplace_back(point);
    m_points = BaseConverter::simplify(m_points);
    if (m_points.size() >= 2) m_canUpdateLine = true;
    this->clearOverlay();
    this->updateLine();
}

std::vector<Point> FreeBrushDrawer::simplify(std::vector<Point> const& points) {
    if (points.size() < 2) return points;

    float maxDistance = 0;
    size_t maxIndex = 0;
    auto perpendicuarDistanceFunc = [](auto const p1, auto const p2, auto const pcheck) {
        auto const l2 = p1.getDistanceSq(p2);
        if (l2 == 0) return pcheck.getDistance(p1);
        auto const t = std::max(0.0, std::min(1.0, (pcheck - p1).dot(p2 - p1) / l2));
        auto const projection = p1 + (p2 - p1) * t;
        return pcheck.getDistance(projection);
    };

    for (size_t i = 1; i < points.size() - 1; ++i) {
        auto const distance = perpendicuarDistanceFunc(points.front(), points.back(), points[i]);
        if (distance > maxDistance) {
            maxDistance = distance;
            maxIndex = i;
        }
    }

    std::vector<Point> simplifiedPoints;
    if (maxDistance <= BrushManager::get()->getFreeThreshold()) {
        simplifiedPoints.emplace_back(points.front());
        simplifiedPoints.emplace_back(points.back());
        return simplifiedPoints;
    }

    auto const left = BaseConverter::simplify(std::vector<Point>(points.begin(), points.begin() + maxIndex + 1));
    auto const right = BaseConverter::simplify(std::vector<Point>(points.begin() + maxIndex, points.end()));

    simplifiedPoints.insert(simplifiedPoints.end(), left.begin(), left.end());
    simplifiedPoints.insert(simplifiedPoints.end(), right.begin() + 1, right.end());

    return simplifiedPoints;
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