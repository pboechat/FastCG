#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

namespace
{
    GLenum GetOpenGLTarget(FastCG::TextureType type)
    {
        switch (type)
        {
        case FastCG::TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled texture type: %d", (int)type);
            return 0;
        }
    }

    GLint GetOpenGLFilter(FastCG::TextureFilter filter)
    {
        switch (filter)
        {
        case FastCG::TextureFilter::POINT_FILTER:
            return GL_NEAREST;
        case FastCG::TextureFilter::LINEAR_FILTER:
            return GL_LINEAR;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled filter: %d", (int)filter);
            return 0;
        }
    }

    GLint GetOpenGLWrapMode(FastCG::TextureWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case FastCG::TextureWrapMode::CLAMP:
            return GL_CLAMP_TO_EDGE;
        case FastCG::TextureWrapMode::REPEAT:
            return GL_REPEAT;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled wrapping mode: %d", (int)wrapMode);
            return 0;
        }
    }

    GLint GetOpenGLInternalFormat(FastCG::TextureFormat format, const FastCG::BitsPerPixel &bitsPerPixel, FastCG::TextureDataType dataType)
    {
        switch (format)
        {
        case FastCG::TextureFormat::R:
            if (bitsPerPixel.r == 8)
            {
                return GL_R8;
            }
            else if (bitsPerPixel.r == 16)
            {
                if (dataType == FastCG::TextureDataType::FLOAT)
                {
                    return GL_R16F;
                }
                else
                {
                    return GL_R16;
                }
            }
            else if (bitsPerPixel.r == 32 && dataType == FastCG::TextureDataType::FLOAT)
            {
                return GL_R32F;
            }
            else
            {
                return GL_RED;
            }
        case FastCG::TextureFormat::RG:
            if (bitsPerPixel.r == 8 && bitsPerPixel.g == 8)
            {
                return GL_RG8;
            }
            else if (bitsPerPixel.r == 16 && bitsPerPixel.g == 16)
            {
                return GL_RG16;
            }
            else
            {
                return GL_RG;
            }
        case FastCG::TextureFormat::RGB:
        case FastCG::TextureFormat::BGR:
            if (bitsPerPixel.r == 8 && bitsPerPixel.g == 8 && bitsPerPixel.b == 8)
            {
                return GL_RGB8;
            }
            else if (bitsPerPixel.r == 10 && bitsPerPixel.g == 10 && bitsPerPixel.b == 10)
            {
                return GL_RGB10;
            }
            else if (bitsPerPixel.r == 12 && bitsPerPixel.g == 12 && bitsPerPixel.b == 12)
            {
                return GL_RGB12;
            }
            else if (bitsPerPixel.r == 16 && bitsPerPixel.g == 16 && bitsPerPixel.b == 16)
            {
                return GL_RGB16;
            }
            else
            {
                return GL_RG;
            }
        case FastCG::TextureFormat::RGBA:
        case FastCG::TextureFormat::BGRA:
            if (bitsPerPixel.r == 32 && bitsPerPixel.g == 32 && bitsPerPixel.b == 32 && bitsPerPixel.a == 32 && dataType == FastCG::TextureDataType::FLOAT)
            {
                return GL_RGBA32F;
            }
            else
            {
                return GL_RGBA;
            }
        case FastCG::TextureFormat::DEPTH_STENCIL:
            if (bitsPerPixel.r == 24 && bitsPerPixel.g == 8)
            {
                return GL_DEPTH24_STENCIL8;
            }
            else
            {
                return GL_DEPTH_STENCIL;
            }
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled texture format: %d", (int)format);
            return 0;
        }
    }

    GLenum GetOpenGLFormat(FastCG::TextureFormat format)
    {
        switch (format)
        {
        case FastCG::TextureFormat::R:
            return GL_RED;
        case FastCG::TextureFormat::RG:
            return GL_RG;
        case FastCG::TextureFormat::RGB:
            return GL_RGB;
        case FastCG::TextureFormat::RGBA:
            return GL_RGBA;
        case FastCG::TextureFormat::BGR:
            return GL_BGR;
        case FastCG::TextureFormat::BGRA:
            return GL_BGRA;
        case FastCG::TextureFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled texture format: %d", (int)format);
            return 0;
        }
    }

    GLenum GetOpenGLDataType(FastCG::TextureDataType dataType, const FastCG::BitsPerPixel &bitsPerPixel)
    {
        switch (dataType)
        {
        case FastCG::TextureDataType::FLOAT:
            return GL_FLOAT;
        case FastCG::TextureDataType::UNSIGNED_CHAR:
            return GL_UNSIGNED_BYTE;
        case FastCG::TextureDataType::UNSIGNED_INT:
            if (bitsPerPixel.r == 24 && bitsPerPixel.g == 8)
            {
                return GL_UNSIGNED_INT_24_8;
            }
            else
            {
                return GL_UNSIGNED_INT;
            }
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled data type: %d", (int)dataType);
            return 0;
        }
    }

    bool IsDepthFormat(FastCG::TextureFormat format)
    {
        return format == FastCG::TextureFormat::DEPTH_STENCIL;
    }

}

namespace FastCG
{
    OpenGLTexture::OpenGLTexture(const TextureArgs &rArgs) : BaseTexture(rArgs)
    {
        auto target = GetOpenGLTarget(mType);
        auto filter = GetOpenGLFilter(mFilter);
        auto wrapMode = GetOpenGLWrapMode(mWrapMode);
        auto internalFormat = GetOpenGLInternalFormat(mFormat, mBitsPerPixel, mDataType);
        auto format = GetOpenGLFormat(mFormat);
        auto dataType = GetOpenGLDataType(mDataType, mBitsPerPixel);

        glGenTextures(1, &mTextureId);
        glBindTexture(target, mTextureId);
#ifdef _DEBUG
        {
            std::string textureLabel = GetName() + " (GL_TEXTURE)";
            glObjectLabel(GL_TEXTURE, mTextureId, (GLsizei)textureLabel.size(), textureLabel.c_str());
        }
#endif
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
        if (IsDepthFormat(mFormat))
        {
            glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }
        glTexImage2D(target, 0, internalFormat, (GLsizei)mWidth, (GLsizei)mHeight, 0, format, dataType, rArgs.pData);

        if (rArgs.generateMipmaps)
        {
            glGenerateMipmap(target);
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