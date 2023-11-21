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
        glTexImage2D(target, 0, internalFormat, (GLsizei)mWidth, (GLsizei)mHeight, 0, format, GetOpenGLDataType(mFormat, mBitsPerChannel), rArgs.pData);
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
