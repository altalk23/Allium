#include <util/converter/TriangulatorConverter.hpp>

#include <poly2tri/poly2tri.h>
#include <boost/geometry.hpp>
#include <correct.hpp>

using namespace allium;
using namespace geode::prelude;

TriangulatorConverter::TriangulatorConverter(std::vector<Polygon>&& polygons, double detail, bool handleHoles)
    : m_polygons(std::move(polygons)), m_detail(detail), m_handleHoles(handleHoles) {}

std::vector<std::unique_ptr<Object>> TriangulatorConverter::handleExtension() {
    std::vector<std::unique_ptr<Object>> triangles;
    
    size_t polygonIndex = 0;
    for (auto const& polygon : m_polygons) {
        using BoostPoint = boost::geometry::model::d2::point_xy<double>;
        using BoostPolygon = boost::geometry::model::polygon<BoostPoint>;
        using BoostRing = boost::geometry::model::ring<BoostPoint>;
        using BoostHole = boost::geometry::model::ring<BoostPoint, false>;
        using BoostMultiPolygon = boost::geometry::model::multi_polygon<BoostPolygon>; 

        BoostMultiPolygon multiPolygon;
        std::vector<std::pair<BoostRing, double>> rings;

        for (auto const& contour : polygon) {
            BoostRing ring;
            for (auto const& point : contour) {
                ring.push_back(BoostPoint(point.x, point.y));
            }
            // log::debug("Processing contour with {} points", ring.size());
            auto fixedRings = geometry::impl::correct(ring, boost::geometry::order_undetermined, 0.0);
            // log::debug("Found {} rings after correction", fixedRings.size());
            for (auto& [fixedRing, area] : fixedRings) {
                boost::geometry::correct(fixedRing);
                // log::debug("Fixed ring has {} points, area: {}", fixedRing.size(), area);
                boost::geometry::simplify(fixedRing, ring, m_detail);
                // log::debug("Simplified ring has {} points", ring.size());
                rings.emplace_back(std::move(ring), area);
            }
        }

        for (size_t i = 0; i < rings.size(); ++i) {
            auto const& [outerRing, outerArea] = rings[i];
            if (m_handleHoles && outerArea < 0) continue;
            // log::debug("Processing ring with {} points, area: {}", outerRing.size(), outerArea);

            bool covered = false;
            for (size_t j = 0; j < rings.size(); ++j) {
                auto const& [coverRing, coverArea] = rings[j];
                if (coverArea < 0) continue;
                if (j == i) continue;
                if (boost::geometry::covered_by(outerRing, coverRing)) {
                    // log::debug("Ring with {} points is covered by another ring with {} points", outerRing.size(), coverRing.size());
                    covered = true;
                    break;
                }
            }
            if (covered) {
                continue;
            }
        

            BoostPolygon boostPolygon;
            boostPolygon.outer() = outerRing;

            if (m_handleHoles) for (size_t j = 0; j < rings.size(); ++j) {
                auto& [innerRing, innerArea] = rings[j];
                if (innerArea >= 0) continue;
                if (j == i) continue;
                // log::debug("Processing hole with {} points, area: {}", innerRing.size(), innerArea);
                if (boost::geometry::covered_by(innerRing, outerRing)) {
                    // log::debug("Has a hole with {} points, area: {}", innerRing.size(), innerArea);
                    boostPolygon.inners().push_back(innerRing);
                }
            }

            multiPolygon.push_back(std::move(boostPolygon));
        }

        BoostMultiPolygon fixedMultiPolygon;
        geometry::correct(multiPolygon, fixedMultiPolygon, 1e-6);
        multiPolygon = std::move(fixedMultiPolygon);

        for (auto const& ring : multiPolygon) {
            // log::debug("Processing ring with {} points", ring.outer().size());

            std::vector<std::unique_ptr<p2t::Point>> storage;
            std::vector<p2t::Point*> polyline;

            // convert the points into the format
            polyline.clear();
            for (size_t i = 0; i < ring.outer().size() - 1; ++i) {
                auto const& point = ring.outer()[i];
                // log::debug("Adding point ({}, {})", point.x(), point.y());
                storage.push_back(std::make_unique<p2t::Point>(point.x(), point.y()));
                polyline.push_back(storage.back().get());
            }

            // i hate this but it uses exceptions
            try {
                p2t::CDT cdt{polyline};
                for (auto const& hole : ring.inners()) {
                    // log::debug("Has a hole with {} points", hole.size());

                    auto const centroid = boost::geometry::return_centroid<BoostPoint>(hole);

                    // convert the points into the format
                    polyline.clear();
                    for (size_t i = 0; i < hole.size() - 1; ++i) {
                        auto const& point = hole[i];
                        // log::debug("Adding hole point ({}, {})", point.x(), point.y());
                        storage.push_back(std::make_unique<p2t::Point>(
                            point.x() * 0.9999 + centroid.x() * 0.0001, point.y() * 0.9999 + centroid.y() * 0.0001
                        ));
                        polyline.push_back(storage.back().get());
                    }
                    cdt.AddHole(polyline);
                }

                cdt.Triangulate();

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
            catch (std::exception const& e) {
                log::error("Failed to triangulate polygon: {}", e.what());
                continue;
            }
        }

        ++polygonIndex;
    }

    return std::move(triangles);
}