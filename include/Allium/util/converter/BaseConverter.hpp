#pragma once

#include <vector>
#include <Geode/Geode.hpp>

#include "../../data/Geometry.hpp"

namespace allium {
    class BaseConverter {
    public:
        BaseConverter() = default;
        virtual ~BaseConverter() = default;
        virtual std::vector<std::unique_ptr<Object>> handleExtension() = 0;

        static std::vector<Point> simplify(std::vector<Point> const& points);
    };
}