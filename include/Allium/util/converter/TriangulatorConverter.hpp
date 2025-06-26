#pragma once

#include "BaseConverter.hpp"

namespace allium {
    class TriangulatorConverter : public BaseConverter {
    public:
        using Contour = std::vector<Point>;
        using Polygon = std::vector<Contour>;
    private:
        std::vector<Polygon> m_polygons;
        bool m_handleHoles;

    public:
        TriangulatorConverter(std::vector<Polygon>&& polygons, bool handleHoles = true);
        ~TriangulatorConverter() override = default;

        std::vector<std::unique_ptr<Object>> handleExtension() override;
    };
}