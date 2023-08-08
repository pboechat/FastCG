#ifndef FASTCG_OPENGL_SHADER_H
#define FASTCG_OPENGL_SHADER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/BaseShader.h>

#include <unordered_map>
#include <string>

namespace FastCG
{
    struct OpenGLResourceInfo
    {
        GLint location;
        GLint binding;
    };

    class OpenGLGraphicsSystem;

    class OpenGLShader : public BaseShader
    {
    public:
        OpenGLShader(const ShaderArgs &rArgs);
        OpenGLShader(const OpenGLShader &rOther) = delete;
        OpenGLShader(const OpenGLShader &&rOther) = delete;
        virtual ~OpenGLShader();

        OpenGLShader operator=(const OpenGLShader &rOther) = delete;

        inline GLuint GetProgramId() const
        {
            return mProgramId;
        }
        inline OpenGLResourceInfo GetResourceInfo(const std::string &rName) const
        {
            auto it = mResourceInfo.find(rName);
            if (it != mResourceInfo.end())
            {
                return it->second;
            }
            else
            {
                return {-1, -1};
            }
        }

    private:
        GLuint mProgramId{~0u};
        ShaderTypeValueArray<GLuint> mShadersIds{};
        std::unordered_map<std::string, OpenGLResourceInfo> mResourceInfo;

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif