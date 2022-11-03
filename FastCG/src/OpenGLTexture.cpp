#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLUtils.h>
#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLExceptions.h>

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