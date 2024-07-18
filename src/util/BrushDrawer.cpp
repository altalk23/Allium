#include <util/BrushDrawer.hpp>
#include <manager/BrushManager.hpp>
#include <util/EditorUtilities.hpp>

using namespace geode::prelude;
using namespace allium;

bool BrushDrawer::init() {
    m_overlay = cocos2d::CCDrawNode::create();
    this->addChild(m_overlay);

    return true;
}

bool BrushDrawer::handleTouchStart(cocos2d::CCPoint const& point) {
    m_canUpdateLine = true;
    return false;
}
void BrushDrawer::handleTouchMove(cocos2d::CCPoint const& point) {}
void BrushDrawer::handleTouchEnd(cocos2d::CCPoint const& point) {}

void BrushDrawer::updateOverlay() {
    auto optimizer = this->initializeConverter();
    std::vector<PolylineConverter::Rect> rects;
    std::vector<PolylineConverter::Circle> circles;
    optimizer.handleExtension(rects, circles);

    auto lineColor = BrushManager::get()->getColor();

    for (auto const& rect : rects) {
        m_overlay->drawPolygon(
            std::array<cocos2d::CCPoint, 4>{
                {ccp(rect.p1.x, rect.p1.y), ccp(rect.p2.x, rect.p2.y), 
                ccp(rect.p3.x, rect.p3.y), ccp(rect.p4.x, rect.p4.y)}
            }.data(),
            4,
            ccc4FFromccc3B(lineColor),
            0,
            ccColor4F{
                0.f, 0.f, 0.f, 0.f
            }
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
            ccc4FFromccc3B(lineColor),
            0,
            ccc4FFromccc3B(lineColor)
        );
    }
}
void BrushDrawer::clearOverlay() {
    m_overlay->clear();
}

void BrushDrawer::updateLine() {
    m_canUpdateLine = false;

    auto optimizer = this->initializeConverter();
    std::vector<PolylineConverter::Rect> rects;
    std::vector<PolylineConverter::Circle> circles;
    optimizer.handleExtension(rects, circles);

    auto const lineColorID = BrushManager::get()->getColorID();

    EditorUtilities::get()->setColor(lineColorID);

    for (auto const& rect : rects) {
        EditorUtilities::get()->addRectangle(
            ccp(rect.p1.x, rect.p1.y),
            ccp(rect.p2.x, rect.p2.y),
            ccp(rect.p3.x, rect.p3.y),
            ccp(rect.p4.x, rect.p4.y)
        );
    }
    for (auto const& circle : circles) {
        EditorUtilities::get()->addCircle(
            ccp(circle.center.x, circle.center.y),
            circle.radius
        );
    }

    EditorUtilities::get()->finish();
}