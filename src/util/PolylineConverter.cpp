#include <util/PolylineConverter.hpp>

using namespace allium;
using namespace geode::prelude;

// Returns the a coefficient of the line equation of the line passing through p1 and p2
double PolylineConverter::a(Point p1, Point p2) {
    return p1.y - p2.y;
}

// Returns the b coefficient of the line equation of the line passing through p1 and p2
double PolylineConverter::b(Point p1, Point p2) {
    return p2.x - p1.x;
}

// Returns the c coefficient of the line equation of the line passing through p1 and p2
double PolylineConverter::c(Point p1, Point p2) {
    return (p1.x - p2.x) * p1.y + (p2.y - p1.y) * p1.x;
}

// Returns the intersection point of the lines passing through p1 and p2 and r1 and r2
PolylineConverter::Point PolylineConverter::intersect(Point p1, Point p2, Point r1, Point r2) {
    auto const a1 = a(p1, p2);
    auto const b1 = b(p1, p2);
    auto const c1 = c(p1, p2);

    auto const a2 = a(r1, r2);
    auto const b2 = b(r1, r2);
    auto const c2 = c(r1, r2);
    return Point{
        (b1 * c2 - b2 * c1) / (a1 * b2 - a2 * b1), (c1 * a2 - c2 * a1) / (a1 * b2 - a2 * b1)
    };
}

// Returns the angle between the lines passing through p1, p2, and p2, p3
double PolylineConverter::angle(Point p1, Point p2, Point p3) {
    auto norm1 = Point{p1.x - p2.x, p1.y - p2.y} / this->distance(p1, p2);
    auto norm2 = Point{p3.x - p2.x, p3.y - p2.y} / this->distance(p2, p3);
    auto dot = norm1.x * norm2.x + norm1.y * norm2.y;
    auto det = norm1.x * norm2.y - norm1.y * norm2.x;
    return std::atan2(det, dot);
}

// Returns the distance between two points
double PolylineConverter::distance(Point p1, Point p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

// Checks if the angle between the lines passing through p1, p2, and p2, p3 is valid
bool PolylineConverter::hasAngle(Point p1, Point p2, Point p3) {
    return this->distance(p1, p2) > 1e-3 && this->distance(p2, p3) > 1e-3;
}

// Returns the offset point of p2 based on the angle between the lines passing through p1, p2, and p2, p3
// This is the corner of the rectangle that is wanted to be extended to intersection point
PolylineConverter::Point PolylineConverter::offset(Point p1, Point p2, Point p3) {
    auto const angle = this->angle(p1, p2, p3);
    auto const slope12 = (p2.y - p1.y) / (p2.x - p1.x);
    if (angle < 0) {
        // right side
        auto const diff = p1 - p2;
        auto const perp = Point{-diff.y, diff.x};
        auto const norm = perp / this->distance(p1, p2);
        return norm * m_lineWidth / 2;
    }
    else {
        // left side
        auto const diff = p1 - p2;
        auto const rperp = Point{diff.y, -diff.x};
        auto const rnorm = rperp / this->distance(p1, p2);
        return rnorm * m_lineWidth / 2;
    }
}

// Extends the corner based on the angle between the lines passing through p1, p2, and p2, p3
auto PolylineConverter::extend(Point p1, Point p2, Point p3) {
    auto const poff = this->offset(p1, p2, p3);
    auto const roff = this->offset(p3, p2, p1);

    // s1 and s2 are the corners of the first rectangle
    auto const s1 = p1 + poff;
    auto const s2 = p2 + poff;
    // r1 and r2 are the corners of the second rectangle
    auto const r1 = p2 + roff;
    auto const r2 = p3 + roff;

    auto const inter = intersect(s1, s2, r1, r2);

    // Returns the center point of both of the extensions
    return std::make_pair(inter - roff, inter - poff);
}

// Checks if the points are colinear
bool PolylineConverter::checkColinear(Point p1, Point p2, Point p3) {
    return std::abs(a(p1, p2) * b(p2, p3) - a(p2, p3) * b(p1, p2)) < 1e-3;
}

PolylineConverter::PolylineConverter(float lineWidth, std::vector<Point>&& points) : m_lineWidth(lineWidth), m_points(std::move(points)) {}

void PolylineConverter::handleExtension(std::vector<PolylineConverter::Rect>& rects, std::vector<PolylineConverter::Circle>& circles) {
    auto const size = m_points.size();

    for (size_t i = 0; i < size - 1; ++i) {
        auto start = m_points[i];
        auto end = m_points[i + 1];

        if (i > 0) { // we're not at the beginning, so we can try to extend the bottom side
            auto const a1 = m_points[i - 1];
            auto const a2 = m_points[i];
            auto const a3 = m_points[i + 1];

            // if the angle is big enough, we can extend the corner
            if (this->hasAngle(a1, a2, a3) && !this->checkColinear(a1, a2, a3) && std::abs(this->angle(a1, a2, a3)) >= M_PI * 0.75) { 
                start = this->extend(a1, a2, a3).first;
            }
        }

        if (i < size - 2) { // we're not at the end, so we can try to extend the top side
            auto const a2 = m_points[i];
            auto const a3 = m_points[i + 1];
            auto const a4 = m_points[i + 2];

            // if the angle is big enough, we can extend the corner
            if (this->hasAngle(a2, a3, a4) && !this->checkColinear(a2, a3, a4) && std::abs(this->angle(a2, a3, a4)) >= M_PI * 0.75) {
                end = this->extend(a2, a3, a4).second;
            }
            else if (this->hasAngle(a2, a3, a4)) {
                // otherwise we need to be content with a circle
                auto const center = end;
                auto const radius = m_lineWidth / 2.0f;
                circles.push_back(Circle{center, radius});
            }
        }

        // we can now create the rectangle
        auto const angle = std::atan2(end.y - start.y, end.x - start.x);
        auto const corner1 = start + Point{
            std::cos(angle + M_PI / 2) * m_lineWidth / 2, 
            std::sin(angle + M_PI / 2) * m_lineWidth / 2
        };
        auto const corner2 = start + Point{
            std::cos(angle - M_PI / 2) * m_lineWidth / 2, 
            std::sin(angle - M_PI / 2) * m_lineWidth / 2
        };
        auto const corner3 = end + Point{
            std::cos(angle - M_PI / 2) * m_lineWidth / 2, 
            std::sin(angle - M_PI / 2) * m_lineWidth / 2
        };
        auto const corner4 = end + Point{
            std::cos(angle + M_PI / 2) * m_lineWidth / 2, 
            std::sin(angle + M_PI / 2) * m_lineWidth / 2
        };

        rects.push_back(Rect{corner1, corner2, corner3, corner4});
    }
}