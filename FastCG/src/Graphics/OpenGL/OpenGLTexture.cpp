#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLErrorHandling.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>

namespace FastCG
{
    OpenGLTexture::OpenGLTexture(const Args &rArgs) : BaseTexture(rArgs)
    {
        auto glTarget = GetOpenGLTarget(GetType());

        FASTCG_CHECK_OPENGL_CALL(glGenTextures(1, &mTextureId));

        FASTCG_CHECK_OPENGL_CALL(glBindTexture(glTarget, mTextureId));
#if _DEBUG
        {
            std::string textureLabel = GetName() + " (GL_TEXTURE)";
            FASTCG_CHECK_OPENGL_CALL(
                glObjectLabel(GL_TEXTURE, mTextureId, (GLsizei)textureLabel.size(), textureLabel.c_str()));
        }
#endif

        auto glFilter = GetOpenGLFilter(GetFilter());
        auto glWrapMode = GetOpenGLWrapMode(GetWrapMode());

        FASTCG_CHECK_OPENGL_CALL(glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, glFilter));
        FASTCG_CHECK_OPENGL_CALL(glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, glFilter));
        FASTCG_CHECK_OPENGL_CALL(glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, glWrapMode));
        FASTCG_CHECK_OPENGL_CALL(glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, glWrapMode));
        FASTCG_CHECK_OPENGL_CALL(glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, glWrapMode));
        if (IsDepthFormat(GetFormat()))
        {
            FASTCG_CHECK_OPENGL_CALL(glTexParameteri(glTarget, GL_TEXTURE_COMPARE_MODE, GL_NONE));
        }

        auto glInternalFormat = GetOpenGLInternalFormat(GetFormat());
        if (IsCompressed(GetFormat()))
        {
            switch (GetType())
            {
#if defined GL_TEXTURE_1D
            case TextureType::TEXTURE_1D: {
                size_t dataOffset = 0;
                for (uint8_t mip = 0; mip < GetMipCount(); ++mip)
                {
                    auto mipDataSize = GetMipDataSize(mip);
                    FASTCG_CHECK_OPENGL_CALL(glCompressedTexImage1D(glTarget, (GLint)mip, glInternalFormat,
                                                                    (GLsizei)GetWidth(mip), 0, (GLsizei)mipDataSize,
                                                                    rArgs.pData + dataOffset));
                    dataOffset += mipDataSize;
                }
            }
            break;
#endif
            case TextureType::TEXTURE_2D: {
                size_t dataOffset = 0;
                for (uint8_t mip = 0; mip < GetMipCount(); ++mip)
                {
                    auto mipDataSize = GetMipDataSize(mip);
                    FASTCG_CHECK_OPENGL_CALL(glCompressedTexImage2D(glTarget, (GLint)mip, glInternalFormat,
                                                                    (GLsizei)GetWidth(mip), (GLsizei)GetHeight(mip), 0,
                                                                    (GLsizei)mipDataSize, rArgs.pData + dataOffset));
                    dataOffset += mipDataSize;
                }
            }
            break;
            case TextureType::TEXTURE_3D: {
                // TODO: support 3D mips
                FASTCG_CHECK_OPENGL_CALL(glCompressedTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(),
                                                                (GLsizei)GetHeight(), (GLsizei)GetDepth(), 0,
                                                                (GLsizei)GetDataSize(), rArgs.pData));
            }
            break;
            case TextureType::TEXTURE_CUBE_MAP: {
                // TODO: support cubemap mips
                size_t dataOffset = 0;
                for (size_t slice = 0; slice < GetSlices(); ++slice)
                {
                    auto sliceDataSize = GetSliceDataSize();
                    FASTCG_CHECK_OPENGL_CALL(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)slice, 0,
                                                                    glInternalFormat, GetWidth(), GetHeight(), 0,
                                                                    (GLsizei)sliceDataSize, rArgs.pData + dataOffset));
                    dataOffset += sliceDataSize;
                }
            }
            break;
            case TextureType::TEXTURE_2D_ARRAY: {
                // TODO: support array mips
                FASTCG_CHECK_OPENGL_CALL(glCompressedTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(),
                                                                (GLsizei)GetHeight(), (GLsizei)GetSlices(), 0,
                                                                (GLsizei)GetDataSize(), rArgs.pData));
            }
            break;
            // TODO: support other array dimensions
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Don't know how to create compressed texture image (texture: %s)",
                                       rArgs.name.c_str());
                break;
            }
        }
        else
        {
            auto glFormat = GetOpenGLFormat(GetFormat());
            auto glDataType = GetOpenGLDataType(GetFormat());
            switch (GetType())
            {
#if defined GL_TEXTURE_1D
            case TextureType::TEXTURE_1D: {
                size_t dataOffset = 0;
                for (uint8_t mip = 0; mip < GetMipCount(); dataOffset += GetMipDataSize(mip), ++mip)
                {
                    FASTCG_CHECK_OPENGL_CALL(glTexImage1D(glTarget, (GLint)mip, glInternalFormat,
                                                          (GLsizei)GetWidth(mip), 0, glFormat, glDataType,
                                                          rArgs.pData + dataOffset));
                }
            }
            break;
#endif
            case TextureType::TEXTURE_2D: {
                size_t dataOffset = 0;
                for (uint8_t mip = 0; mip < GetMipCount(); dataOffset += GetMipDataSize(mip), ++mip)
                {
                    FASTCG_CHECK_OPENGL_CALL(glTexImage2D(glTarget, (GLint)mip, glInternalFormat,
                                                          (GLsizei)GetWidth(mip), (GLsizei)GetHeight(mip), 0, glFormat,
                                                          glDataType, rArgs.pData + dataOffset));
                }
            }
            break;
            case TextureType::TEXTURE_3D: {
                // TODO: support 3D mips
                FASTCG_CHECK_OPENGL_CALL(glTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(),
                                                      (GLsizei)GetHeight(), (GLsizei)GetDepth(), 0, glFormat,
                                                      glDataType, rArgs.pData));
            }
            break;
            case TextureType::TEXTURE_CUBE_MAP: {
                // TODO: support cubemap mips
                for (size_t slice = 0, dataOffset = 0; slice < GetSlices(); ++slice, dataOffset += GetSliceDataSize())
                {
                    FASTCG_CHECK_OPENGL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)slice, 0,
                                                          glInternalFormat, GetWidth(), GetHeight(), 0, glFormat,
                                                          glDataType, rArgs.pData + dataOffset));
                }
            }
            break;
            case TextureType::TEXTURE_2D_ARRAY: {
                // TODO: support array mips
                FASTCG_CHECK_OPENGL_CALL(glTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(),
                                                      (GLsizei)GetHeight(), (GLsizei)GetSlices(), 0, glFormat,
                                                      glDataType, rArgs.pData));
            }
            break;
            // TODO: support other array dimensions
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Don't know how to create texture image (texture: %s)",
                                       rArgs.name.c_str());
                break;
            }
        }

        if (rArgs.generateMips)
        {
            FASTCG_CHECK_OPENGL_CALL(glGenerateMipmap(glTarget));
        }
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
