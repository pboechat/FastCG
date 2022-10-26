#ifndef FASTCG_OPENGL_MESH_H
#define FASTCG_OPENGL_MESH_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLBuffer.h>
#include <FastCG/BaseMesh.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLMesh : public BaseMesh
    {
    public:
        void SetVertices(const std::vector<glm::vec3> &vertices);
        void SetNormals(const std::vector<glm::vec3> &normals);
        void SetUVs(const std::vector<glm::vec2> &uvs);
        void SetTangents(const std::vector<glm::vec4> &tangents);
        void SetColors(const std::vector<glm::vec4> &colors);
        void SetIndices(const std::vector<uint32_t> &indices);
        void Draw() const;

    private:
        GLuint mVertexArrayId{~0u};
        OpenGLBuffer *mpVerticesBuffer{nullptr};
        OpenGLBuffer *mpNormalsBuffer{nullptr};
        OpenGLBuffer *mpUVsBuffer{nullptr};
        OpenGLBuffer *mpTangentsBuffer{nullptr};
        OpenGLBuffer *mpColorsBuffer{nullptr};
        OpenGLBuffer *mpIndicesBuffer{nullptr};

        OpenGLMesh(const MeshArgs &rArgs);
        virtual ~OpenGLMesh();

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif