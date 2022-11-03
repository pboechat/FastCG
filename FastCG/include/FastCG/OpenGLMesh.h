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

    class OpenGLMesh : public BaseMesh<OpenGLBuffer>
    {
    private:
        OpenGLMesh(const MeshArgs &rArgs);
        virtual ~OpenGLMesh();

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif