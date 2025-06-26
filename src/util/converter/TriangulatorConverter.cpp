#include <util/converter/TriangulatorConverter.hpp>

#include <clipper2/clipper.h>
#include <poly2tri/poly2tri.h>

using namespace allium;
using namespace geode::prelude;

TriangulatorConverter::TriangulatorConverter(std::vector<Polygon>&& polygons, bool handleHoles)
    : m_polygons(std::move(polygons)), m_handleHoles(handleHoles) {}

std::vector<std::unique_ptr<Object>> TriangulatorConverter::handleExtension() {
    std::vector<std::unique_ptr<Object>> triangles;
    
    size_t polygonIndex = 0;
    for (auto const& polygon : m_polygons) {
        Clipper2Lib::PathsD contours;

        for (auto const& contour : polygon) {
            if (contour.size() < 3) continue; // Skip polygons with less than 3 points
            Clipper2Lib::PathD path;
            for (auto const& point : contour) {
                path.emplace_back(point.x, point.y);
            }
            if (!m_handleHoles && !Clipper2Lib::IsPositive(path)) {
               path = Clipper2Lib::PathD(path.rbegin(), path.rend());
            }
            contours.push_back(std::move(path));
        }

        auto result = Clipper2Lib::Union(contours, Clipper2Lib::FillRule::NonZero);

        // log::debug("Found {} contours in polygon {}", result.size(), polygonIndex);
        for (size_t i = 0; i < result.size(); ++i) {
            auto const& contour = result[i];
            std::vector<std::unique_ptr<p2t::Point>> storage;
            std::vector<p2t::Point*> polyline;

            // convert the points into the format
            polyline.clear();
            for (auto const& point : contour) {
                storage.push_back(std::make_unique<p2t::Point>(point.x, point.y));
                polyline.push_back(storage.back().get());
            }
            p2t::CDT cdt{polyline};
            auto j = i;
            while (++j < result.size()) {
                auto const& hole = result[j];
                if (Clipper2Lib::IsPositive(hole)) break;

                // convert the points into the format
                polyline.clear();
                for (auto const& point : hole) {
                    storage.push_back(std::make_unique<p2t::Point>(point.x, point.y));
                    polyline.push_back(storage.back().get());
                }
                cdt.AddHole(polyline);
                ++i;
            }

            // i hate this but it uses exceptions
            try {
                cdt.Triangulate();
            }
            catch (std::exception const& e) {
                log::error("Failed to triangulate polygon: {}", e.what());
            }

            for (auto const& triangle : cdt.GetTriangles()) {
                auto p0 = triangle->GetPoint(0);
                auto p1 = triangle->GetPoint(1);
                auto p2 = triangle->GetPoint(2);
                triangles.push_back(std::make_unique<Triangle>(
                    Point(p0->x, p0->y), Point(p1->x, p1->y), Point(p2->x, p2->y)
                ));
                triangles.back()->idx = polygonIndex;
            }
        }

        ++polygonIndex;
    }

    return std::move(triangles);
}