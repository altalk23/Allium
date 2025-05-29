#include <util/converter/TriangulatorConverter.hpp>

using namespace allium;
using namespace geode::prelude;

TriangulatorConverter::TriangulatorConverter(std::vector<Polygon>&& polygons)
    : m_polygons(std::move(polygons)) {}

void TriangulatorConverter::converterBegin(GLenum type) {
    m_primitiveType = type;
    m_currentVertices.clear();
}
void TriangulatorConverter::converterVertex(void* vertexData) {
    auto const* doubleData = static_cast<GLdouble*>(vertexData);
    m_currentVertices.emplace_back(
        doubleData[0], doubleData[1]
    );
}
void TriangulatorConverter::converterEnd() {
    switch (m_primitiveType) {
        case GL_TRIANGLES: {
            if (m_currentVertices.size() < 3) return; // Not enough vertices to form a triangle
            for (size_t i = 0; i < m_currentVertices.size() - 2; i += 3) {
                m_triangles.push_back(std::make_unique<Triangle>(
                    m_currentVertices[i], m_currentVertices[i + 1], m_currentVertices[i + 2]
                ));
            }
            break;
        }
        case GL_TRIANGLE_FAN: {
            if (m_currentVertices.size() < 3) return; // Not enough vertices to form a triangle fan
            auto center = m_currentVertices[0];
            for (size_t i = 1; i < m_currentVertices.size() - 1; ++i) {
                m_triangles.push_back(std::make_unique<Triangle>(
                    center, m_currentVertices[i], m_currentVertices[i + 1]
                ));
            }
            break;
        }
        case GL_TRIANGLE_STRIP: {
            if (m_currentVertices.size() < 3) return; // Not enough vertices to form a triangle strip
            for (size_t i = 0; i < m_currentVertices.size() - 2; ++i) {
                if (i % 2 == 0) {
                    m_triangles.push_back(std::make_unique<Triangle>(
                        m_currentVertices[i], m_currentVertices[i + 1], m_currentVertices[i + 2]
                    ));
                } else {
                    m_triangles.push_back(std::make_unique<Triangle>(
                        m_currentVertices[i + 2], m_currentVertices[i + 1], m_currentVertices[i]
                    ));
                }
            }
            break;
        }
        default:
            log::error("Unsupported primitive type: {}", m_primitiveType);
    }

    m_currentVertices.clear();
}
void TriangulatorConverter::converterCombine(GLdouble coords[3], void* vertexData, GLfloat weight[4], void** outData) {
    auto newVertex = std::make_unique<double[]>(3);
    newVertex[0] = coords[0];
    newVertex[1] = coords[1];
    newVertex[2] = coords[2];
    *outData = newVertex.get();
    m_allocatedData.push_back(std::move(newVertex));
}
void TriangulatorConverter::converterError(GLenum errorCode) {
    const GLubyte* errorStr = gluErrorString(errorCode);
    log::error("Tessellation error: {}", errorStr ? reinterpret_cast<const char*>(errorStr) : "Unknown error");
}

std::vector<std::unique_ptr<Object>> TriangulatorConverter::handleExtension() {
    m_triangles.clear();
    
    auto tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (void (*)()) +[](GLenum type, void* polygonData) {
        auto self = static_cast<TriangulatorConverter*>(polygonData);
        self->converterBegin(type);
    });
    gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (void (*)()) +[](void* vertexData, void* polygonData) {
        auto self = static_cast<TriangulatorConverter*>(polygonData);
        self->converterVertex(vertexData);
    });
    gluTessCallback(tess, GLU_TESS_END_DATA, (void (*)()) +[](void* polygonData) {
        auto self = static_cast<TriangulatorConverter*>(polygonData);
        self->converterEnd();
    });
    gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (void (*)()) +[](GLdouble coords[3], void* vertexData, GLfloat weight[4], void** outData, void* polygonData) {
        auto self = static_cast<TriangulatorConverter*>(polygonData);
        self->converterCombine(coords, vertexData, weight, outData);
    });
    gluTessCallback(tess, GLU_TESS_ERROR_DATA, (void (*)()) +[](GLenum errorCode, void* polygonData) {
        auto self = static_cast<TriangulatorConverter*>(polygonData);
        self->converterError(errorCode);
    });

    gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    

    for (auto const& polygon : m_polygons) {
        gluTessBeginPolygon(tess, this);

        for (auto const& contour : polygon) {
            if (contour.size() < 3) continue; // Skip polygons with less than 3 points

            gluTessBeginContour(tess);
            
            for (const auto& vertex : contour) {
                auto data = std::make_unique<double[]>(3);
                data[0] = vertex.x;
                data[1] = vertex.y;
                data[2] = 0.0;
                gluTessVertex(tess, data.get(), data.get());
                m_allocatedData.push_back(std::move(data));
            }
            
            gluTessEndContour(tess);
        }

        gluTessEndPolygon(tess);
    }

    gluDeleteTess(tess);
    m_allocatedData.clear();

    return std::move(m_triangles);
}