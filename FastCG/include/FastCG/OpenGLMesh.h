#ifndef FASTCG_OPENGL_MESH_H
#define FASTCG_OPENGL_MESH_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseMesh.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;
    class OpenGLForwardRenderingPathStrategy;
    class OpenGLDeferredRenderingPathStrategy;

    class OpenGLMesh : public BaseMesh
    {
    private:
        GLuint mVertexArrayId{~0u};
        GLuint mVerticesBufferId{~0u};
        GLuint mNormalsBufferId{~0u};
        GLuint mUVsBufferId{~0u};
        GLuint mTangentsBufferId{~0u};
        GLuint mColorsBufferId{~0u};
        GLuint mIndicesBufferId{~0u};

        OpenGLMesh(const MeshArgs &rArgs);
        virtual ~OpenGLMesh();

        void SetVertices(const std::vector<glm::vec3> &vertices);
        void SetNormals(const std::vector<glm::vec3> &normals);
        void SetUVs(const std::vector<glm::vec2> &uvs);
        void SetTangents(const std::vector<glm::vec4> &tangents);
        void SetColors(const std::vector<glm::vec4> &colors);
        void SetIndices(const std::vector<uint32_t> &indices);

        void Draw() const;

        friend class OpenGLRenderingSystem;
        friend class OpenGLForwardRenderingPathStrategy;
        friend class OpenGLDeferredRenderingPathStrategy;
    };

}

#endif

#endif