#pragma once

#include "BaseConverter.hpp"

namespace allium {
    class TriangulatorConverter : public BaseConverter {
    public:
        using Contour = std::vector<Point>;
        using Polygon = std::vector<Contour>;
    private:
        std::vector<Polygon> m_polygons;

        GLenum m_primitiveType = 0;
        std::vector<Point> m_currentVertices;
        std::vector<std::unique_ptr<Object>> m_triangles;
        std::vector<std::unique_ptr<double[]>> m_allocatedData;

        void converterBegin(GLenum type);
        void converterVertex(void* vertexData);
        void converterEnd();
        void converterCombine(GLdouble coords[3], void* vertexData, GLfloat weight[4], void** outData);
        void converterError(GLenum errorCode);

    public:
        TriangulatorConverter(std::vector<Polygon>&& polygons);
        ~TriangulatorConverter() override = default;

        std::vector<std::unique_ptr<Object>> handleExtension() override;
    };
}