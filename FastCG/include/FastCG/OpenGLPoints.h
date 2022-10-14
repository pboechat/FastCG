#ifndef FASTCG_OPENGL_POINTS_H
#define FASTCG_OPENGL_POINTS_H

#ifdef FASTCG_OPENGL

#include <FastCG/BasePoints.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLPoints : public BasePoints
    {
    public:
        virtual ~OpenGLPoints();

        void Draw() override;

    private:
        GLuint mVertexArrayId{~0u};
        GLuint mVerticesBufferId{~0u};
        GLuint mColorsBufferId{~0u};

        void AllocateResources();
        void DeallocateResources();
    };

}

#endif

#endif