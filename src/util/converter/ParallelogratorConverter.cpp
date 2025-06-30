#include <util/converter/ParallelogratorConverter.hpp>


using namespace allium;
using namespace geode::prelude;

#include <boost/geometry.hpp>

ParallelogratorConverter::ParallelogratorConverter(std::vector<Polygon>&& polygons, double detail, bool handleHoles)
    : TriangulatorConverter(std::move(polygons), detail, handleHoles) {}

ParallelogratorConverter::~ParallelogratorConverter() = default;

std::vector<std::vector<std::unique_ptr<Object>>> ParallelogratorConverter::handleExtension() {
    auto trianglesList = TriangulatorConverter::handleExtension();

    using BoostPoint = boost::geometry::model::d2::point_xy<double>;
    using BoostMultiPoint = boost::geometry::model::multi_point<BoostPoint>;
    using BoostRing = boost::geometry::model::ring<BoostPoint>;

    auto const vectorBetween = [](BoostPoint const& a, BoostPoint const& b) {
        return BoostPoint(b.x() - a.x(), b.y() - a.y());
    };

    auto const cross = [](BoostPoint const& a, BoostPoint const& b) {
        return a.x() * b.y() - a.y() * b.x();
    };

    auto const areParallel = [&](BoostPoint const& a, BoostPoint const& b, BoostPoint const& c, BoostPoint const& d) {
        auto const ab = vectorBetween(a, b);
        auto const cd = vectorBetween(c, d);
        return std::abs(cross(ab, cd)) < 1e-2;
    };

    std::vector<std::vector<std::unique_ptr<Object>>> newObjects;

    for (auto& triangles : trianglesList) {
        std::unordered_set<size_t> processedTriangles;
        newObjects.emplace_back();

        // better than nothing honestly
        for (size_t i = 0; i < triangles.size(); ++i) {
            if (processedTriangles.count(i)) continue;

            auto const& t1 = static_cast<Triangle const&>(*triangles[i]);
            auto const& p1 = t1.p1;
            auto const& p2 = t1.p2;
            auto const& p3 = t1.p3;

            for (size_t j = i + 1; j < triangles.size(); ++j) {
                if (processedTriangles.count(i)) continue;

                auto const& t2 = static_cast<Triangle const&>(*triangles[j]);
                auto const& q1 = t2.p1;
                auto const& q2 = t2.p2;
                auto const& q3 = t2.p3;

                BoostMultiPoint mp;
                mp.push_back(BoostPoint(p1.x, p1.y));
                mp.push_back(BoostPoint(p2.x, p2.y));
                mp.push_back(BoostPoint(p3.x, p3.y));
                mp.push_back(BoostPoint(q1.x, q1.y));
                mp.push_back(BoostPoint(q2.x, q2.y));
                mp.push_back(BoostPoint(q3.x, q3.y));

                size_t equalCount = 0;
                if (p1 == q1) ++equalCount;
                if (p1 == q2) ++equalCount;
                if (p1 == q3) ++equalCount;
                if (p2 == q1) ++equalCount;
                if (p2 == q2) ++equalCount;
                if (p2 == q3) ++equalCount;
                if (p3 == q1) ++equalCount;
                if (p3 == q2) ++equalCount;
                if (p3 == q3) ++equalCount;
                if (equalCount != 2) continue; // need 2 equals

                BoostRing hull;
                boost::geometry::convex_hull(mp, hull);
                if (hull.size() == 5) {
                    auto const& w1 = hull[0];
                    auto const& w2 = hull[1];
                    auto const& w3 = hull[2];
                    auto const& w4 = hull[3];

                    if (areParallel(w1, w2, w3, w4) && areParallel(w2, w3, w4, w1)) {
                        processedTriangles.insert(i);
                        processedTriangles.insert(j);

                        newObjects.back().push_back(std::make_unique<Parallelogram>(
                            Point(w1.x(), w1.y()), 
                            Point(w2.x(), w2.y()), 
                            Point(w3.x(), w3.y()), 
                            Point(w4.x(), w4.y())
                        ));
                        break;
                    }
                }
            }
        }

        for (size_t i = 0; i < triangles.size(); ++i) {
            if (!processedTriangles.count(i)) {
                newObjects.back().push_back(std::move(triangles[i]));
            }
        }
    }

    return newObjects;
}