#include <util/CurveBrushDrawer.hpp>
#include <agg-2.6/agg_curves.h>
#include "CurveOptimizer.hpp"

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
    m_canUpdateLine = true;
    if (m_points.size() == 2) {
        m_points[1] = point;
    } else {
        auto const point2Mirror = m_points[3] + (m_points[3] - point);
        m_points[2] = point2Mirror;
        m_currentPoints = this->getGeneratedPoints();
    }
    this->updateOverlay();
}
void CurveBrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {
    CurveBrushDrawer::handleTouchMove(point);
    this->updateOverlay();
    m_previousPoints.insert(m_previousPoints.end(), m_currentPoints.begin(), m_currentPoints.end());
    m_currentPoints.clear();
}

std::vector<std::array<double, 2>> CurveBrushDrawer::getGeneratedPoints() {
    std::vector<std::array<double, 2>> points;
    auto generator = agg::curve4_div();
    generator.approximation_scale(m_curveRoughness);
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

CurveOptimizer CurveBrushDrawer::initalizeOptimizer() {
    std::vector<CurveOptimizer::Point> points;
    for (auto const& point : m_previousPoints) {
        points.push_back({point[0], point[1]});
    }
    for (auto const& point : m_currentPoints) {
        points.push_back({point[0], point[1]});
    }
    return CurveOptimizer(m_lineWidth, std::move(points));
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
       
        auto optimizer = this->initalizeOptimizer();
        std::vector<CurveOptimizer::Rect> rects;
        std::vector<CurveOptimizer::Circle> circles;
        optimizer.handleExtension(rects, circles);

        for (auto const& rect : rects) {
            m_overlay->drawPolygon(
                std::array<cocos2d::CCPoint, 4>{
                    {ccp(rect.p1.x, rect.p1.y), ccp(rect.p2.x, rect.p2.y), 
                    ccp(rect.p3.x, rect.p3.y), ccp(rect.p4.x, rect.p4.y)}
                }.data(),
                4,
                ccc4FFromccc3B(m_lineColor),
                0,
                ccc4FFromccc3B(m_lineColor)
            );
        }
        for (auto const& circle : circles) {
            std::vector<cocos2d::CCPoint> points;
            for (size_t i = 0; i < 64; ++i) {
                auto const angle = i * 2 * M_PI / 64;
                points.emplace_back(ccp(circle.center.x, circle.center.y) + CCPoint::forAngle(angle) * circle.radius);
            }
            m_overlay->drawPolygon(
                points.data(),
                points.size(),
                ccc4FFromccc3B(m_lineColor),
                0,
                ccc4FFromccc3B(m_lineColor)
            );
        }
    }   
}

void CurveBrushDrawer::updateLine() {
    if (!m_canUpdateLine) return;
    m_canUpdateLine = false;

    auto optimizer = this->initalizeOptimizer();
    std::vector<CurveOptimizer::Rect> rects;
    std::vector<CurveOptimizer::Circle> circles;
    optimizer.handleExtension(rects, circles);

    for (auto const& rect : rects) {
        static constexpr int SQUARE_OBJECT_ID = 211;
        static constexpr int WHITE_COLOR_ID = 1011;
        static constexpr float SQUARE_OBJECT_SCALE = 30.f;

        auto const center = (ccp(rect.p1.x, rect.p1.y) + ccp(rect.p3.x, rect.p3.y)) / 2.0f;
        auto const angle = std::atan2(rect.p2.y - rect.p1.y, rect.p2.x - rect.p1.x);

        auto object = LevelEditorLayer::get()->createObject(SQUARE_OBJECT_ID, center, false);
        object->setRotation(-angle * 180.0f / M_PI);

        auto scaleX = ccp(rect.p1.x, rect.p1.y).getDistance(ccp(rect.p2.x, rect.p2.y)) / SQUARE_OBJECT_SCALE;
        auto scaleY = ccp(rect.p1.x, rect.p1.y).getDistance(ccp(rect.p4.x, rect.p4.y)) / SQUARE_OBJECT_SCALE;

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
    for (auto const& circle : circles) {
        static constexpr int CIRCLE_OBJECT_ID = 725;
        static constexpr int WHITE_COLOR_ID = 1011;
        static constexpr float CIRCLE_OBJECT_SCALE = 9.f;

        auto object = LevelEditorLayer::get()->createObject(CIRCLE_OBJECT_ID, ccp(circle.center.x, circle.center.y), false);
        
        auto scaleX = circle.radius * 2 / CIRCLE_OBJECT_SCALE;
        auto scaleY = circle.radius * 2 / CIRCLE_OBJECT_SCALE;

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

    m_previousPoints.clear();
    m_currentPoints.clear();
    m_points.clear();
}