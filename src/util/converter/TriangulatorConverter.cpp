#include <util/converter/TriangulatorConverter.hpp>

#include <poly2tri/poly2tri.h>
#include <boost/geometry.hpp>
#include <correct.hpp>
#include <random>

using namespace allium;
using namespace geode::prelude;

TriangulatorConverter::TriangulatorConverter(std::vector<Polygon>&& polygons, double detail, bool handleHoles)
    : m_polygons(std::move(polygons)), m_detail(detail), m_handleHoles(handleHoles) {}

double TriangulatorConverter::getArea(Point const& p1, Point const& p2, Point const& p3) {
    // triangle area
    return 0.5 * ((p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y));
}

std::vector<std::vector<std::unique_ptr<Object>>> TriangulatorConverter::handleExtension() {
    std::vector<std::vector<std::unique_ptr<Object>>> triangles;

    for (auto& polygon : m_polygons) {
        triangles.emplace_back();
        using BoostPoint = boost::geometry::model::d2::point_xy<double>;
        using BoostPolygon = boost::geometry::model::polygon<BoostPoint>;
        using BoostRing = boost::geometry::model::ring<BoostPoint>;
        using BoostHole = boost::geometry::model::ring<BoostPoint, false>;
        using BoostMultiPolygon = boost::geometry::model::multi_polygon<BoostPolygon>; 

        BoostMultiPolygon multiPolygon;
        std::vector<std::pair<BoostRing, double>> rings;

        auto const calculateContours = [&](Contour const& contour, double& totalArea) {
            BoostRing ring;
            for (auto const& point : contour) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                Point entropyPoint = Point(
                    std::uniform_real_distribution<>(-1e-4, 1e-4)(gen), 
                    std::uniform_real_distribution<>(-1e-4, 1e-4)(gen)
                );
                ring.push_back(BoostPoint(
                    point.x + entropyPoint.x,
                    point.y + entropyPoint.y
                ));
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
                totalArea += area;
            }
        };

        double totalArea = 0.0;
        for (auto const& contour : polygon) {
            calculateContours(contour, totalArea);
        }

        if (totalArea < 0) { 
            // its most likely a funny font that does not obey cw/ccw rule, lets just flip it
            rings.clear();
            totalArea = 0.0;
            for (auto& contour : polygon) {
                std::reverse(contour.begin(), contour.end());
                calculateContours(contour, totalArea);
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
        geometry::correct(multiPolygon, fixedMultiPolygon, 1e-3);
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

                    // convert the points into the format
                    polyline.clear();
                    for (size_t i = 0; i < hole.size() - 1; ++i) {
                        auto const& point = hole[i];
                        // log::debug("Adding hole point ({}, {})", point.x(), point.y());
                        storage.push_back(std::make_unique<p2t::Point>(point.x(), point.y()));
                        polyline.push_back(storage.back().get());
                    }
                    cdt.AddHole(polyline);
                }

                cdt.Triangulate();

                for (auto const& triangle : cdt.GetTriangles()) {
                    auto p0 = triangle->GetPoint(0);
                    auto p1 = triangle->GetPoint(1);
                    auto p2 = triangle->GetPoint(2);

                    auto r0 = Point(p0->x, p0->y);
                    auto r1 = Point(p1->x, p1->y);
                    auto r2 = Point(p2->x, p2->y);
                    if (std::abs(this->getArea(r0, r1, r2)) < 1e-2) {
                        continue; // skip degenerate triangles
                    }

                    triangles.back().push_back(std::make_unique<Triangle>(
                        r0, r1, r2
                    ));
                }
            }
            catch (std::exception const& e) {
                log::error("Failed to triangulate polygon: {}", e.what());
                continue;
            }
        }
    }

    return std::move(triangles);
}