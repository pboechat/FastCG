#ifndef FASTCG_OPENGL_MESH_H
#define FASTCG_OPENGL_MESH_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseMesh.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLMesh : public BaseMesh
    {
    public:
        virtual ~OpenGLMesh();

        void Draw() override;

    private:
        GLuint mVertexArrayId{~0u};
        GLuint mVerticesBufferId{~0u};
        GLuint mNormalsBufferId{~0u};
        GLuint mUVsBufferId{~0u};
        GLuint mTangentsBufferId{~0u};
        GLuint mIndicesBufferId{~0u};

        void AllocateResources();
        void DeallocateResources();
    };

}

#endif

#endif