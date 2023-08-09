#ifdef FASTCG_OPENGL

#include <FastCG/Platform/FileReader.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Core/StringUtils.h>
#include <FastCG/Core/MsgBox.h>

namespace
{
#define DECLARE_CHECK_STATUS_FN(object)                                                                                          \
    void Check##object##Status(GLuint objectId, GLenum status, const std::string &rIdentifier)                                   \
    {                                                                                                                            \
        GLint statusValue;                                                                                                       \
        glGet##object##iv(objectId, status, &statusValue);                                                                       \
        GLint infoLogLength;                                                                                                     \
        glGet##object##iv(objectId, GL_INFO_LOG_LENGTH, &infoLogLength);                                                         \
        if (infoLogLength > 0)                                                                                                   \
        {                                                                                                                        \
            std::string infoLog;                                                                                                 \
            infoLog.reserve(infoLogLength);                                                                                      \
            glGet##object##InfoLog(objectId, infoLogLength, &infoLogLength, &infoLog[0]);                                        \
            if (statusValue == GL_TRUE)                                                                                          \
            {                                                                                                                    \
                FASTCG_MSG_BOX("OpenGLShader", #object " info log ('%s'): %s", rIdentifier.c_str(), infoLog.c_str())             \
            }                                                                                                                    \
            else                                                                                                                 \
            {                                                                                                                    \
                FASTCG_THROW_EXCEPTION(FastCG::Exception, #object " info log ('%s'): %s", rIdentifier.c_str(), infoLog.c_str()); \
            }                                                                                                                    \
        }                                                                                                                        \
    }

    DECLARE_CHECK_STATUS_FN(Shader)
    DECLARE_CHECK_STATUS_FN(Program)

    void GetResourceLocations(GLuint programId, std::unordered_map<std::string, FastCG::OpenGLResourceInfo> &rResourceInfos)
    {
        for (GLenum iface : {GL_UNIFORM_BLOCK, GL_SHADER_STORAGE_BLOCK, GL_UNIFORM})
        {
            GLint numActiveResources = 0;
            glGetProgramInterfaceiv(programId, iface, GL_ACTIVE_RESOURCES, &numActiveResources);
            FASTCG_CHECK_OPENGL_ERROR();

            for (GLint i = 0; i < numActiveResources; ++i)
            {
                GLsizei length = 0;
                GLchar buffer[128];

                glGetProgramResourceName(programId, iface, i, FASTCG_ARRAYSIZE(buffer), &length, buffer);
                FASTCG_CHECK_OPENGL_ERROR();

                std::string resourceName(buffer, length);

                auto it = rResourceInfos.find(resourceName);
                if (it != rResourceInfos.end())
                {
                    continue;
                }

                GLenum property;
                GLint location = -1;
                GLint binding = -1;
                GLint type = -1;
                if (iface == GL_UNIFORM)
                {
                    property = GL_LOCATION;
                    glGetProgramResourceiv(programId, iface, i, 1, &property, 1, nullptr, &location);
                    FASTCG_CHECK_OPENGL_ERROR();

                    if (location == -1)
                    {
                        continue;
                    }

                    glGetUniformiv(programId, location, &binding);

                    property = GL_TYPE;
                    glGetProgramResourceiv(programId, iface, i, 1, &property, 1, nullptr, &type);
                }
                else
                {
                    property = GL_BUFFER_BINDING;
                    glGetProgramResourceiv(programId, iface, i, 1, &property, 1, nullptr, &binding);
                }
                FASTCG_CHECK_OPENGL_ERROR();

                if (binding == -1)
                {
                    continue;
                }

                rResourceInfos[resourceName] = {location, binding, iface, type};
            }
        }
    }

}

namespace FastCG
{
    OpenGLShader::OpenGLShader(const ShaderArgs &rArgs) : BaseShader(rArgs)
    {
        std::fill(mShadersIds.begin(), mShadersIds.end(), ~0u);
        for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::LAST; ++i)
        {
            auto shaderType = (ShaderType)i;

            const auto &rProgramData = rArgs.programsData[i];
            if (rProgramData.dataSize == 0)
            {
                continue;
            }

            auto glShaderType = GetOpenGLShaderType(shaderType);
            auto shaderId = glCreateShader(glShaderType);

            if (rArgs.text)
            {
                glShaderSource(shaderId, 1, (const char **)&rProgramData.pData, nullptr);
                glCompileShader(shaderId);
            }
            else
            {
                if (GLEW_ARB_gl_spirv)
                {
                    glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, rProgramData.pData, (GLsizei)rProgramData.dataSize);
                    glSpecializeShaderARB(shaderId, "main", 0, nullptr, nullptr);
                }
                else
                {
                    FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Cannot use SPIR-V shaders!");
                }
            }

            CheckShaderStatus(shaderId, GL_COMPILE_STATUS, mName);

            mShadersIds[i] = shaderId;

            FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
            std::string shaderLabel = GetName() + " (" + GetOpenGLShaderTypeString(glShaderType) + ")";
            glObjectLabel(GL_SHADER, shaderId, (GLsizei)shaderLabel.size(), shaderLabel.c_str());
#endif
        }

        mProgramId = glCreateProgram();

        for (const auto &shaderId : mShadersIds)
        {
            glAttachShader(mProgramId, shaderId);
        }

        glLinkProgram(mProgramId);
        CheckProgramStatus(mProgramId, GL_LINK_STATUS, GetName());

        GetResourceLocations(mProgramId, mResourceInfo);

        for (const auto &shaderId : mShadersIds)
        {
            glDetachShader(mProgramId, shaderId);
        }

#ifdef _DEBUG
        glValidateProgram(mProgramId);
        CheckProgramStatus(mProgramId, GL_VALIDATE_STATUS, GetName());
#endif

#ifdef _DEBUG
        std::string programLabel = GetName() + " (GL_PROGRAM)";
        glObjectLabel(GL_PROGRAM, mProgramId, (GLsizei)programLabel.size(), programLabel.c_str());
#endif
    }

    OpenGLShader::~OpenGLShader()
    {
        for (auto shaderId : mShadersIds)
        {
            glDeleteShader(shaderId);
        }

        if (mProgramId != ~0u)
        {
            glDeleteProgram(mProgramId);
        }
    }

}

#endif
