#ifdef FASTCG_OPENGL

#include <FastCG/ShaderSource.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/FastCG.h>

#include <algorithm>

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

#define CASE_RETURN_STRING(str) \
    case str:                   \
        return #str
    const char *GetOpenGLShaderTypeString(GLenum shaderType)
    {
        switch (shaderType)
        {
            CASE_RETURN_STRING(GL_VERTEX_SHADER);
            CASE_RETURN_STRING(GL_FRAGMENT_SHADER);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpenGL shader type");
            return nullptr;
        }
    }

    GLenum GetOpenGLShaderType(FastCG::ShaderType shaderType)
    {
        switch (shaderType)
        {
        case FastCG::ShaderType::ST_VERTEX:
            return GL_VERTEX_SHADER;
        case FastCG::ShaderType::ST_FRAGMENT:
            return GL_FRAGMENT_SHADER;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled shader type");
            return 0;
        }
    }

}

namespace FastCG
{
    OpenGLShader::OpenGLShader(const ShaderArgs &rName) : BaseShader(rName)
    {
        std::fill(mShadersIds.begin(), mShadersIds.end(), ~0u);
        for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::ST_MAX; i++)
        {
            auto shaderType = (ShaderType)i;
            const auto &shaderFileName = mArgs.shaderFileNames[i];

            if (shaderFileName.empty())
            {
                continue;
            }

            auto shaderSource = ShaderSource::Parse(shaderFileName);
            auto glShaderType = GetOpenGLShaderType(shaderType);
            auto shaderId = glCreateShader(glShaderType);

            if (shaderSource.empty())
            {
                FASTCG_THROW_EXCEPTION(Exception, "OpenGLShader file not found: %s", shaderFileName.c_str());
            }

            const auto *pShaderSource = shaderSource.c_str();
            glShaderSource(shaderId, 1, (const char **)&pShaderSource, 0);

            glCompileShader(shaderId);
            CheckShaderStatus(shaderId, GL_COMPILE_STATUS, shaderFileName);

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

        glBindAttribLocation(mProgramId, VERTICES_ATTRIBUTE_INDEX, "position");
        glBindAttribLocation(mProgramId, NORMALS_ATTRIBUTE_INDEX, "normal");
        glBindAttribLocation(mProgramId, UVS_ATTRIBUTE_INDEX, "uv");
        glBindAttribLocation(mProgramId, TANGENTS_ATTRIBUTE_INDEX, "tangent");
        glBindAttribLocation(mProgramId, COLORS_ATTRIBUTE_INDEX, "color");

        glLinkProgram(mProgramId);
        CheckProgramStatus(mProgramId, GL_LINK_STATUS, GetName());

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

    void OpenGLShader::Bind() const
    {
        glUseProgram(mProgramId);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

    void OpenGLShader::BindTexture(GLint bindingLocation, GLuint textureId, GLint textureUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUniform1i(bindingLocation, textureUnit);
    }

}

#endif