#ifndef FASTCG_OPENGL_SHADER_H
#define FASTCG_OPENGL_SHADER_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseShader.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLShader : public BaseShader
    {
    public:
        inline operator GLuint() const
        {
            return mProgramId;
        }

    private:
        GLuint mProgramId{~0u};
        ShaderTypeValueArray<GLuint> mShadersIds{};

        OpenGLShader(const ShaderArgs &rName);
        OpenGLShader(const OpenGLShader &rOther) = delete;
        OpenGLShader(const OpenGLShader &&rOther) = delete;
        virtual ~OpenGLShader();

        OpenGLShader operator=(const OpenGLShader &rOther) = delete;

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif