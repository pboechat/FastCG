#ifdef FASTCG_OPENGL

#include <FastCG/Core/CollectionUtils.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/StringUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLErrorHandling.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Platform/FileReader.h>

#include <cstddef>
#include <vector>

namespace
{
#define DECLARE_CHECK_STATUS_FN(object)                                                                                \
    bool Check##object##Status(GLuint objectId, GLenum status, std::string &rInfoLog)                                  \
    {                                                                                                                  \
        GLint statusValue;                                                                                             \
        glGet##object##iv(objectId, status, &statusValue);                                                             \
        if (!statusValue)                                                                                              \
        {                                                                                                              \
            GLint infoLogLength;                                                                                       \
            glGet##object##iv(objectId, GL_INFO_LOG_LENGTH, &infoLogLength);                                           \
            rInfoLog.resize(infoLogLength + 1);                                                                        \
            if (infoLogLength > 0)                                                                                     \
            {                                                                                                          \
                glGet##object##InfoLog(objectId, infoLogLength, &infoLogLength, &rInfoLog[0]);                         \
            }                                                                                                          \
            rInfoLog[infoLogLength] = '\0';                                                                            \
            return false;                                                                                              \
        }                                                                                                              \
        return true;                                                                                                   \
    }

    DECLARE_CHECK_STATUS_FN(Shader)
    DECLARE_CHECK_STATUS_FN(Program)

    void GetShaderResourceLocations(const std::string &rIdentifier, GLuint programId,
                                    std::unordered_map<std::string, FastCG::OpenGLResourceInfo> &rResourceInfos)
    {
        FASTCG_UNUSED(rIdentifier);
        for (GLenum iface : {GL_UNIFORM_BLOCK, GL_SHADER_STORAGE_BLOCK, GL_UNIFORM})
        {
            GLint activeResourcesCount = 0;
            FASTCG_CHECK_OPENGL_CALL(
                glGetProgramInterfaceiv(programId, iface, GL_ACTIVE_RESOURCES, &activeResourcesCount));

            for (GLint i = 0; i < activeResourcesCount; ++i)
            {
                GLsizei length = 0;
                GLchar buffer[128];

                FASTCG_CHECK_OPENGL_CALL(
                    glGetProgramResourceName(programId, iface, i, FASTCG_ARRAYSIZE(buffer), &length, buffer));

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
                    FASTCG_CHECK_OPENGL_CALL(
                        glGetProgramResourceiv(programId, iface, i, 1, &property, 1, nullptr, &location));

                    if (location == -1)
                    {
                        continue;
                    }

                    FASTCG_CHECK_OPENGL_CALL(glGetUniformiv(programId, location, &binding));

                    property = GL_TYPE;
                    FASTCG_CHECK_OPENGL_CALL(
                        glGetProgramResourceiv(programId, iface, i, 1, &property, 1, nullptr, &type));
                }
                else
                {
                    property = GL_BUFFER_BINDING;
                    FASTCG_CHECK_OPENGL_CALL(
                        glGetProgramResourceiv(programId, iface, i, 1, &property, 1, nullptr, &binding));
                }

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
    OpenGLShader::OpenGLShader(const Args &rArgs) : BaseShader(rArgs)
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
            FASTCG_CHECK_OPENGL_ERROR("Failed to create shader");
            assert(shaderId != 0);

            if (rArgs.text)
            {
                FASTCG_CHECK_OPENGL_CALL(glShaderSource(shaderId, 1, (const char **)&rProgramData.pData, nullptr));
                FASTCG_CHECK_OPENGL_CALL(glCompileShader(shaderId));

                std::string infoLog;
                if (!CheckShaderStatus(shaderId, GL_COMPILE_STATUS, infoLog))
                {
                    std::vector<std::string> tokens;
                    StringUtils::Split(infoLog, ":", tokens);
                    assert(tokens.size() >= 4);

                    std::vector<std::string> lines;
                    StringUtils::Split((const char *)rProgramData.pData, "\n", lines);

                    auto errorLine = (size_t)atoll(tokens[1].c_str());
                    assert(errorLine > 0);
                    assert(lines.size() > errorLine);
                    std::string snippet;
                    if (errorLine > 1)
                    {
                        snippet = "    " + lines[errorLine - 2] + "\n";
                    }
                    snippet += "--> " + lines[errorLine - 1] + "\n";
                    if (lines.size() > errorLine)
                    {
                        snippet += "    " + lines[errorLine] + "\n";
                    }

                    std::vector<std::string> subTokens;
                    CollectionUtils::Slice(tokens, 2, tokens.size(), subTokens);
                    auto cause = StringUtils::Join(subTokens, ":");

                    FASTCG_THROW_EXCEPTION(
                        FastCG::Exception, "Couldn't compile shader module (program: %s, type: %s):\n%s\n%s",
                        mName.c_str(), GetShaderTypeString(shaderType), cause.c_str(), snippet.c_str());
                }
            }
#if defined FASTCG_ANDROID
            else
            {
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't find shader code (program: %s, type: %s)", mName.c_str(),
                                       GetShaderTypeString(shaderType));
            }
#else
            else
            {
                if (GLEW_ARB_gl_spirv)
                {
                    FASTCG_CHECK_OPENGL_CALL(glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB,
                                                            rProgramData.pData, (GLsizei)rProgramData.dataSize));
                    FASTCG_CHECK_OPENGL_CALL(glSpecializeShaderARB(shaderId, "main", 0, nullptr, nullptr));
                }
                else
                {
                    FASTCG_THROW_EXCEPTION(Exception, "Can't use shader SPIR-V module (program: %s, type: %s)",
                                           mName.c_str(), GetShaderTypeString(shaderType));
                }

                std::string infoLog;
                if (!CheckShaderStatus(shaderId, GL_COMPILE_STATUS, infoLog))
                {
                    FASTCG_THROW_EXCEPTION(Exception, "Couldn't compile shader module (program: %s, type: %s):\n%s",
                                           mName.c_str(), GetShaderTypeString(shaderType), infoLog.c_str());
                }
            }
#endif

            mShadersIds[i] = shaderId;

#if _DEBUG
            std::string shaderLabel = GetName() + " (" + GetOpenGLShaderTypeString(glShaderType) + ") (GL_SHADER)";
            FASTCG_CHECK_OPENGL_CALL(
                glObjectLabel(GL_SHADER, shaderId, (GLsizei)shaderLabel.size(), shaderLabel.c_str()));
#endif
        }

        mProgramId = glCreateProgram();
        FASTCG_CHECK_OPENGL_ERROR("Failed to create program");
        assert(mProgramId != 0);

        for (const auto &shaderId : mShadersIds)
        {
            if (shaderId != ~0u)
            {
                FASTCG_CHECK_OPENGL_CALL(glAttachShader(mProgramId, shaderId));
            }
        }

        FASTCG_CHECK_OPENGL_CALL(glLinkProgram(mProgramId));
        {
            std::string infoLog;
            if (!CheckProgramStatus(mProgramId, GL_LINK_STATUS, infoLog))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't link shader program (program: %s):\n%s", mName.c_str(),
                                       infoLog.c_str());
            }
        }

        GetShaderResourceLocations(mName, mProgramId, mResourceInfo);

        for (const auto &shaderId : mShadersIds)
        {
            if (shaderId != ~0u)
            {
                FASTCG_CHECK_OPENGL_CALL(glDetachShader(mProgramId, shaderId));
            }
        }

#if _DEBUG
        FASTCG_CHECK_OPENGL_CALL(glValidateProgram(mProgramId));
        {
            std::string infoLog;
            if (!CheckProgramStatus(mProgramId, GL_VALIDATE_STATUS, infoLog))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't validate shader program (program: %s):\n%s", mName.c_str(),
                                       infoLog.c_str());
            }
        }
#endif

#if _DEBUG
        std::string programLabel = GetName() + " (GL_PROGRAM)";
        FASTCG_CHECK_OPENGL_CALL(
            glObjectLabel(GL_PROGRAM, mProgramId, (GLsizei)programLabel.size(), programLabel.c_str()));
#endif
    }

    OpenGLShader::~OpenGLShader()
    {
        for (auto shaderId : mShadersIds)
        {
            if (shaderId != ~0u)
            {
                glDeleteShader(shaderId);
            }
        }

        if (mProgramId != ~0u)
        {
            glDeleteProgram(mProgramId);
        }
    }

}

#endif
