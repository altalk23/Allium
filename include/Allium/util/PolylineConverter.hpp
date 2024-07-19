#pragma once

#include <stdint.h>
#include <vector>
#include <cmath>
#include <numbers>
#include <Geode/Geode.hpp>

namespace allium {
    class PolylineConverter {
    public:
        struct Point {
            double x;
            double y;

            Point() = default;
            Point(double x, double y)
                : x(x), y(y)
            {};

            Point operator+(Point const& other) const {
                return Point{x + other.x, y + other.y};
            }

            Point operator-(Point const& other) const {
                return Point{x - other.x, y - other.y};
            }

            Point operator*(double const& scalar) const {
                return Point{x * scalar, y * scalar};
            }

            Point operator/(double const& scalar) const {
                return Point{x / scalar, y / scalar};
            }
        };

        struct Rect {
            Point p1;
            Point p2;
            Point p3;
            Point p4;
        };

        struct Circle {
            Point center;
            float radius;
        };
        
    private:
        float m_lineWidth;
        std::vector<Point> m_points;

        // Returns the a coefficient of the line equation of the line passing through p1 and p2
        double a(Point p1, Point p2);

        // Returns the b coefficient of the line equation of the line passing through p1 and p2
		double b(Point p1, Point p2);

        // Returns the c coefficient of the line equation of the line passing through p1 and p2
		double c(Point p1, Point p2);

        // Returns the intersection point of the lines passing through p1 and p2 and r1 and r2
        Point intersect(Point p1, Point p2, Point r1, Point r2);

        // Returns the angle between the lines passing through p1, p2, and p2, p3
        double angle(Point p1, Point p2, Point p3);

        // Returns the distance between two points
        double distance(Point p1, Point p2);

        // Checks if the angle between the lines passing through p1, p2, and p2, p3 is valid
        bool hasAngle(Point p1, Point p2, Point p3);

        // Returns the offset point of p2 based on the angle between the lines passing through p1, p2, and p2, p3
        // This is the corner of the rectangle that is wanted to be extended to intersection point
        Point offset(Point p1, Point p2, Point p3);

        // Extends the corner based on the angle between the lines passing through p1, p2, and p2, p3
        auto extend(Point p1, Point p2, Point p3);

        // Checks if the points are colinear
        bool checkColinear(Point p1, Point p2, Point p3);

    public:

        PolylineConverter(float lineWidth, std::vector<Point>&& points);

        void handleExtension(std::vector<PolylineConverter::Rect>& rects, std::vector<PolylineConverter::Circle>& circles);
    };
}