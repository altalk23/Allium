#include <util/converter/BaseConverter.hpp>
#include <manager/BrushManager.hpp>

using namespace allium;
using namespace geode::prelude;

std::vector<Point> BaseConverter::simplify(std::vector<Point> const& points) {
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

cocos2d::CCPoint BaseConverter::align(cocos2d::CCPoint const& point, cocos2d::CCPoint const& toAlign) {
    auto horizontalDiff = point.x - toAlign.x;
    auto verticalDiff = point.y - toAlign.y;
    if (std::abs(horizontalDiff) > std::abs(verticalDiff)) {
        return cocos2d::CCPoint{toAlign.x, point.y};
    } else {
        return cocos2d::CCPoint{point.x, toAlign.y};
    }
}

cocos2d::CCPoint BaseConverter::gridAlign(cocos2d::CCPoint const& point, float gridSize) {
    auto const x = std::round((point.x + gridSize/2.f) / gridSize - 0.5f) * gridSize;
    auto const y = std::round((point.y + gridSize/2.f) / gridSize - 0.5f) * gridSize;
    return cocos2d::CCPoint{x, y};
}