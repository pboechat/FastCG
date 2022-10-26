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
        void Bind() const;
        void Unbind() const;
        void BindTexture(GLint bindingLocation, GLuint textureId, GLint textureUnit) const;

        inline GLint GetBindingLocation(const std::string &rName) const
        {
            return glGetUniformLocation(mProgramId, rName.c_str());
        }

    private:
        GLuint mProgramId{~0u};
        ShaderTypeValueArray<GLuint> mShadersIds{};

        OpenGLShader(const ShaderArgs &rName);
        virtual ~OpenGLShader();

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif