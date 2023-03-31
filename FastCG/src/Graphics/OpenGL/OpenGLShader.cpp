#ifdef FASTCG_OPENGL

#include <FastCG/ShaderSource.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/FastCG.h>

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

}

namespace FastCG
{
    OpenGLShader::OpenGLShader(const ShaderArgs &rName) : BaseShader(rName)
    {
        std::fill(mShadersIds.begin(), mShadersIds.end(), ~0u);
        for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::MAX; i++)
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

}

#endif