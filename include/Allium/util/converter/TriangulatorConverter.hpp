#pragma once

#include "BaseConverter.hpp"

namespace allium {
    class TriangulatorConverter : public BaseConverter {
    public:
        using Contour = std::vector<Point>;
        using Polygon = std::vector<Contour>;
    private:
        std::vector<Polygon> m_polygons;
        double m_detail;
        bool m_handleHoles;

        double getArea(Point const& p1, Point const& p2, Point const& p3);

    public:
        TriangulatorConverter(std::vector<Polygon>&& polygons, double detail, bool handleHoles = true);
        ~TriangulatorConverter() override = default;

        std::vector<std::vector<std::unique_ptr<Object>>> handleExtension() override;
    };
}