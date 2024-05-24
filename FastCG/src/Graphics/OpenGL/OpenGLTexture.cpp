#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>

namespace FastCG
{
    OpenGLTexture::OpenGLTexture(const Args &rArgs) : BaseTexture(rArgs)
    {
        auto glTarget = GetOpenGLTarget(GetType());

        glGenTextures(1, &mTextureId);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't generate texture (texture: %s)", rArgs.name.c_str());

        glBindTexture(glTarget, mTextureId);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't bind texture (texture: %s)", rArgs.name.c_str());
#if _DEBUG
        {
            std::string textureLabel = GetName() + " (GL_TEXTURE)";
            glObjectLabel(GL_TEXTURE, mTextureId, (GLsizei)textureLabel.size(), textureLabel.c_str());
        }
#endif

        auto glFilter = GetOpenGLFilter(GetFilter());
        auto glWrapMode = GetOpenGLWrapMode(GetWrapMode());

        glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, glFilter);
        glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, glFilter);
        glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, glWrapMode);
        glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, glWrapMode);
        glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, glWrapMode);
        if (IsDepthFormat(GetFormat()))
        {
            glTexParameteri(glTarget, GL_TEXTURE_COMPARE_MODE, GL_NONE);
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
                    glCompressedTexImage1D(glTarget, (GLint)mip, glInternalFormat, (GLsizei)GetWidth(mip), 0,
                                           (GLsizei)mipDataSize, rArgs.pData + dataOffset);
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
                    glCompressedTexImage2D(glTarget, (GLint)mip, glInternalFormat, (GLsizei)GetWidth(mip),
                                           (GLsizei)GetHeight(mip), 0, (GLsizei)mipDataSize, rArgs.pData + dataOffset);
                    dataOffset += mipDataSize;
                }
            }
            break;
            case TextureType::TEXTURE_3D: {
                // TODO: support 3D mips
                glCompressedTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(), (GLsizei)GetHeight(),
                                       (GLsizei)GetDepth(), 0, (GLsizei)GetDataSize(), rArgs.pData);
            }
            break;
            case TextureType::TEXTURE_CUBE_MAP: {
                // TODO: support cubemap mips
                size_t dataOffset = 0;
                for (size_t slice = 0; slice < GetSlices(); ++slice)
                {
                    auto sliceDataSize = GetSliceDataSize();
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)slice, 0, glInternalFormat,
                                           GetWidth(), GetHeight(), 0, (GLsizei)sliceDataSize,
                                           rArgs.pData + dataOffset);
                    dataOffset += sliceDataSize;
                }
            }
            break;
            case TextureType::TEXTURE_2D_ARRAY: {
                // TODO: support array mips
                glCompressedTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(), (GLsizei)GetHeight(),
                                       (GLsizei)GetSlices(), 0, (GLsizei)GetDataSize(), rArgs.pData);
            }
            break;
            // TODO: support other array dimensions
            default:
                FASTCG_CHECK_OPENGL_ERROR("Don't know how to create compressed texture image (texture: %s)",
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
                    glTexImage1D(glTarget, (GLint)mip, glInternalFormat, (GLsizei)GetWidth(mip), 0, glFormat,
                                 glDataType, rArgs.pData + dataOffset);
                }
            }
            break;
#endif
            case TextureType::TEXTURE_2D: {
                size_t dataOffset = 0;
                for (uint8_t mip = 0; mip < GetMipCount(); dataOffset += GetMipDataSize(mip), ++mip)
                {
                    glTexImage2D(glTarget, (GLint)mip, glInternalFormat, (GLsizei)GetWidth(mip),
                                 (GLsizei)GetHeight(mip), 0, glFormat, glDataType, rArgs.pData + dataOffset);
                }
            }
            break;
            case TextureType::TEXTURE_3D: {
                // TODO: support 3D mips
                glTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(), (GLsizei)GetHeight(),
                             (GLsizei)GetDepth(), 0, glFormat, glDataType, rArgs.pData);
            }
            break;
            case TextureType::TEXTURE_CUBE_MAP: {
                // TODO: support cubemap mips
                for (size_t slice = 0, dataOffset = 0; slice < GetSlices(); ++slice, dataOffset += GetSliceDataSize())
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)slice, 0, glInternalFormat, GetWidth(),
                                 GetHeight(), 0, glFormat, glDataType, rArgs.pData + dataOffset);
                }
            }
            break;
            case TextureType::TEXTURE_2D_ARRAY: {
                // TODO: support array mips
                glTexImage3D(glTarget, 0, glInternalFormat, (GLsizei)GetWidth(), (GLsizei)GetHeight(),
                             (GLsizei)GetSlices(), 0, glFormat, glDataType, rArgs.pData);
            }
            break;
            // TODO: support other array dimensions
            default:
                FASTCG_CHECK_OPENGL_ERROR("Don't know how to create texture image (texture: %s)", rArgs.name.c_str());
                break;
            }
        }

        FASTCG_CHECK_OPENGL_ERROR("Couldn't create texture image (texture: %s)", rArgs.name.c_str());

        if (rArgs.generateMips)
        {
            glGenerateMipmap(glTarget);
            FASTCG_CHECK_OPENGL_ERROR("Couldn't generate mips (texture: %s)", rArgs.name.c_str());
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
