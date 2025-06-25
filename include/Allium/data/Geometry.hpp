#pragma once

#include <stdint.h>
#include <cmath>
#include <numbers>
#include <Geode/Geode.hpp>

namespace allium {
    class DrawNodeExtension;
    struct Point {
        double x = 0.0;
        double y = 0.0;

        Point() = default;
        Point(double x, double y)
            : x(x), y(y)
        {};
        Point(cocos2d::CCPoint const& point)
            : x(static_cast<double>(point.x)), y(static_cast<double>(point.y))
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

        double dot(Point const& other) const {
            return x * other.x + y * other.y;
        }

        double perpDot(Point const& other) const {
            return x * other.y - y * other.x;
        }

        double getDistanceSq(Point const& other) const {
            return std::pow(x - other.x, 2) + std::pow(y - other.y, 2);
        }

        double getDistance(Point const& other) const {
            return std::sqrt(this->getDistanceSq(other));
        }

        double angleTo(Point const& other) const {
            double const angle = std::acos(
                this->dot(other) / std::sqrt(this->getLengthSq() * other.getLengthSq())
            );

            return angle * ((this->perpDot(other) < 0) ? -1 : 1);
        }

        double getLengthSq() const {
            return x * x + y * y;
        }

        double getLength() const {
            return std::sqrt(this->getLengthSq());
        }
        
        operator cocos2d::CCPoint() const {
            return cocos2d::CCPoint(static_cast<float>(x), static_cast<float>(y));
        }
    };

    struct Col2 {
        std::array<double, 2> data;
        Col2(double x = 0.0, double y = 0.0)
            : data({x, y}) {}
        Col2(Point const& point)
            : data({point.x, point.y}) {}

        operator Point() const {
            return Point(data[0], data[1]);
        }
    };

    struct Mat2 {
        std::array<Col2, 2> data;

        Mat2(Col2 const& col1, Col2 const& col2)
            : data{col1, col2}
        {};

        Mat2(double a11, double a12, double a21, double a22)
            : data{Col2(a11, a21), Col2(a12, a22)}
        {};

        static Mat2 fromAngle(double angle) {
            double cosA = std::cos(angle);
            double sinA = std::sin(angle);
            return Mat2{Col2(cosA, sinA), Col2(-sinA, cosA)};
        }

        Col2 operator*(Col2 const& vec) const {
            return Col2{
                data[0].data[0] * vec.data[0] + data[1].data[0] * vec.data[1],
                data[0].data[1] * vec.data[0] + data[1].data[1] * vec.data[1]
            };
        }

        Mat2 operator*(Mat2 const& other) const {
            return Mat2{
                Col2{
                    data[0].data[0] * other.data[0].data[0] + data[1].data[0] * other.data[0].data[1],
                    data[0].data[1] * other.data[0].data[0] + data[1].data[1] * other.data[0].data[1]
                },
                Col2{
                    data[0].data[0] * other.data[1].data[0] + data[1].data[0] * other.data[1].data[1],
                    data[0].data[1] * other.data[1].data[0] + data[1].data[1] * other.data[1].data[1]
                }
            };
        }
    };

    struct Object {
        size_t idx = 0;
        virtual ~Object() = default;
        virtual geode::Result<GameObject*> addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const = 0;
        virtual geode::Result<> drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const = 0;
    };

    struct Rect : Object {
        Point p1;
        Point p2;
        Point p3;
        Point p4;

        Rect() = default;
        Rect(Point p1, Point p2, Point p3, Point p4)
            : p1(p1), p2(p2), p3(p3), p4(p4)
        {};
        virtual ~Rect() = default;

        geode::Result<GameObject*> addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const override;
        geode::Result<> drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const override;
    };

    struct Circle : Object {
        Point center;
        float radius;

        Circle() = default;
        Circle(Point center, float radius)
            : center(center), radius(radius)
        {};
        virtual ~Circle() = default;

        geode::Result<GameObject*> addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const override;
        geode::Result<> drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const override;
    };

    struct Triangle : Object {
        Point p1;
        Point p2;
        Point p3;

        Triangle() = default;
        Triangle(Point p1, Point p2, Point p3)
            : p1(p1), p2(p2), p3(p3)
        {};
        virtual ~Triangle() = default;

        geode::Result<GameObject*> addAsGameObject(LevelEditorLayer* editorLayer, int colorID) const override;
        geode::Result<> drawIntoDrawNode(DrawNodeExtension* node, cocos2d::ccColor3B color) const override;
    };
}