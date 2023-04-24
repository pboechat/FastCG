#ifndef FASTCG_OPENGL_SHADER_H
#define FASTCG_OPENGL_SHADER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/BaseShader.h>

#include <string>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLShader : public BaseShader
    {
    public:
        inline operator GLuint() const
        {
            return mProgramId;
        }

        OpenGLShader(const ShaderArgs &rArgs);
        OpenGLShader(const OpenGLShader &rOther) = delete;
        OpenGLShader(const OpenGLShader &&rOther) = delete;
        virtual ~OpenGLShader();

        OpenGLShader operator=(const OpenGLShader &rOther) = delete;

    private:
        GLuint mProgramId{~0u};
        ShaderTypeValueArray<GLuint> mShadersIds{};

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif