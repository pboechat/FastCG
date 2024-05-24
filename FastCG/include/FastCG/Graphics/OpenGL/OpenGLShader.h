#ifndef FASTCG_OPENGL_SHADER_H
#define FASTCG_OPENGL_SHADER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/BaseShader.h>
#include <FastCG/Graphics/OpenGL/OpenGL.h>

#include <string>
#include <unordered_map>

namespace FastCG
{
    struct OpenGLResourceInfo
    {
        GLint location;
        GLint binding;
        GLenum iface;
        GLint type;
    };

    using OpenGLResourceInfoMap = std::unordered_map<std::string, OpenGLResourceInfo>;

    class OpenGLGraphicsSystem;

    class OpenGLShader : public BaseShader
    {
    public:
        OpenGLShader(const Args &rArgs);
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
        const OpenGLResourceInfoMap &GetResourceInfoMap() const
        {
            return mResourceInfo;
        }

    private:
        GLuint mProgramId{~0u};
        ShaderTypeValueArray<GLuint> mShadersIds{};
        OpenGLResourceInfoMap mResourceInfo;

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif