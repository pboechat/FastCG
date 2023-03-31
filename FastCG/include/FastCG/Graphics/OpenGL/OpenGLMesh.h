#ifndef FASTCG_OPENGL_MESH_H
#define FASTCG_OPENGL_MESH_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/BaseMesh.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLMesh : public BaseMesh<OpenGLBuffer>
    {
    private:
        OpenGLMesh(const MeshArgs &rArgs);
        OpenGLMesh(const OpenGLMesh &rOther) = delete;
        OpenGLMesh(const OpenGLMesh &&rOther) = delete;
        virtual ~OpenGLMesh();

        OpenGLMesh operator=(const OpenGLMesh &rOther) = delete;

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif