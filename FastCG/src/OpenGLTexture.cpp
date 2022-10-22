#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

namespace
{
    uint32_t GetOpenGLFilter(FastCG::TextureFilter filter)
    {
        switch (filter)
        {
        case FastCG::TextureFilter::TF_POINT_FILTER:
            return GL_NEAREST;
        case FastCG::TextureFilter::TF_LINEAR_FILTER:
            return GL_LINEAR;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled filter: %d", filter);
            return 0;
        }
    }

    uint32_t GetOpenGLWrapMode(FastCG::TextureWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case FastCG::TextureWrapMode::TW_CLAMP:
            return GL_CLAMP;
        case FastCG::TextureWrapMode::TW_REPEAT:
            return GL_REPEAT;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled wrapping mode: %d", wrapMode);
            return 0;
        }
    }

    uint32_t GetOpenGLFormat(FastCG::TextureFormat format)
    {
        switch (format)
        {
        case FastCG::TextureFormat::TF_R:
            return GL_RED;
        case FastCG::TextureFormat::TF_RG:
            return GL_RG;
        case FastCG::TextureFormat::TF_RGB:
            return GL_RGB;
        case FastCG::TextureFormat::TF_RGBA:
            return GL_RGBA;
        case FastCG::TextureFormat::TF_BGR:
            return GL_BGR;
        case FastCG::TextureFormat::TF_BGRA:
            return GL_BGRA;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled internal format: %d", format);
            return 0;
        }
    }

    uint32_t GetOpenGLDataType(FastCG::TextureDataType dataType)
    {
        switch (dataType)
        {
        case FastCG::TextureDataType::DT_FLOAT:
            return GL_FLOAT;
        case FastCG::TextureDataType::DT_UNSIGNED_CHAR:
            return GL_UNSIGNED_BYTE;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled data type: %d", dataType);
            return 0;
        }
    }

}

namespace FastCG
{
    OpenGLTexture::OpenGLTexture(const TextureArgs &rArgs) : BaseTexture(rArgs)
    {
        auto filter = GetOpenGLFilter(GetFilterMode());
        auto wrapMode = GetOpenGLWrapMode(GetWrapMode());
        auto format = GetOpenGLFormat(GetFormat());
        auto dataType = GetOpenGLDataType(GetDataType());

        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
#ifdef _DEBUG
        {
            std::string textureLabel = GetName() + " (GL_TEXTURE)";
            glObjectLabel(GL_TEXTURE, mTextureId, (GLsizei)textureLabel.size(), textureLabel.c_str());
        }
#endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexImage2D(GL_TEXTURE_2D, 0, format, GetWidth(), GetHeight(), 0, format, dataType, mArgs.pData);

        if (mArgs.generateMipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    OpenGLTexture::~OpenGLTexture()
    {
        if (mTextureId != ~0u)
        {
            glDeleteTextures(1, &mTextureId);
        }
    }
}

#endif