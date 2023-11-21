#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>

namespace FastCG
{
    OpenGLTexture::OpenGLTexture(const Args &rArgs) : BaseTexture(rArgs)
    {
        auto target = GetOpenGLTarget(mType);
        auto filter = GetOpenGLFilter(mFilter);
        auto wrapMode = GetOpenGLWrapMode(mWrapMode);
        auto internalFormat = GetOpenGLInternalFormat(mFormat, mBitsPerChannel, mDataType);
        auto format = GetOpenGLFormat(mFormat);

        glGenTextures(1, &mTextureId);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't generate texture (texture: %s)", rArgs.name.c_str());

        glBindTexture(target, mTextureId);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't bind texture (texture: %s)", rArgs.name.c_str());
#if _DEBUG
        {
            std::string textureLabel = GetName() + " (GL_TEXTURE)";
            glObjectLabel(GL_TEXTURE, mTextureId, (GLsizei)textureLabel.size(), textureLabel.c_str());
        }
#endif
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
        if (IsDepthFormat(mFormat))
        {
            glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }

        auto dataType = GetOpenGLDataType(mFormat, mBitsPerChannel);
        switch (mType)
        {
        case TextureType::TEXTURE_1D:
        {
            size_t dataOffset = 0;
            for (uint8_t mip = 0; mip < GetMipCount(); dataOffset += GetMipDataSize(mip), ++mip)
            {
                glTexImage1D(target, (GLint)mip, internalFormat, (GLsizei)GetWidth(mip), 0, format, dataType, rArgs.pData + dataOffset);
            }
        }
        break;
        case TextureType::TEXTURE_2D:
        {
            size_t dataOffset = 0;
            for (uint8_t mip = 0; mip < GetMipCount(); dataOffset += GetMipDataSize(mip), ++mip)
            {
                glTexImage2D(target, (GLint)mip, internalFormat, (GLsizei)GetWidth(mip), (GLsizei)GetHeight(mip), 0, format, dataType, rArgs.pData + dataOffset);
            }
        }
        break;
        case TextureType::TEXTURE_3D:
        {
            glTexImage3D(target, 0, internalFormat, (GLsizei)GetWidth(), (GLsizei)GetHeight(), (GLsizei)GetDepth(), 0, format, dataType, rArgs.pData);
        }
        break;
        case TextureType::TEXTURE_CUBE_MAP:
        {
            for (size_t slice = 0, dataOffset = 0; slice < GetSlices(); ++slice, dataOffset += GetSliceDataSize())
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)slice, 0, internalFormat, GetWidth(), GetHeight(), 0, format, dataType, rArgs.pData + dataOffset);
            }
        }
        break;
        case TextureType::TEXTURE_2D_ARRAY:
        {
            glTexImage3D(target, 0, internalFormat, (GLsizei)GetWidth(), (GLsizei)GetHeight(), (GLsizei)GetSlices(), 0, format, dataType, rArgs.pData);
        }
        break;
        }
        FASTCG_CHECK_OPENGL_ERROR("Couldn't create texture image (texture: %s)", rArgs.name.c_str());

        if (rArgs.generateMips)
        {
            glGenerateMipmap(target);
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
