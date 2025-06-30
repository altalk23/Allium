#pragma once

#include "TriangulatorConverter.hpp"

namespace allium {
    class ParallelogratorConverter : public TriangulatorConverter {
    public:
        ParallelogratorConverter(std::vector<Polygon>&& polygons, double detail, bool handleHoles = true);
        ~ParallelogratorConverter() override;

        std::vector<std::vector<std::unique_ptr<Object>>> handleExtension() override;
    };
}