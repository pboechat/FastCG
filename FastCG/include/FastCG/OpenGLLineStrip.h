#ifndef FASTCG_OPENGL_LINE_STRIP_H
#define FASTCG_OPENGL_LINE_STRIP_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseLineStrip.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLLineStrip : public BaseLineStrip
    {
    public:
        virtual ~OpenGLLineStrip();

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