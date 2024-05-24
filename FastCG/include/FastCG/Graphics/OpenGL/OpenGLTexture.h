#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/BaseTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGL.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLTexture : public BaseTexture
    {
    public:
        struct Args : BaseTexture::Args
        {
            bool generateMips;

            Args(const std::string &rName = "", uint32_t width = 1, uint32_t height = 1, uint32_t depthOrSlices = 1,
                 uint8_t mipCount = 1, TextureType type = TextureType::TEXTURE_2D,
                 TextureUsageFlags usage = TextureUsageFlagBit::SAMPLED,
                 TextureFormat format = TextureFormat::R8G8B8A8_UNORM,
                 TextureFilter filter = TextureFilter::LINEAR_FILTER, TextureWrapMode wrapMode = TextureWrapMode::CLAMP,
                 const uint8_t *pData = nullptr, bool generateMips = false)
                : BaseTexture::Args(rName, width, height, depthOrSlices, mipCount, type, usage, format, filter,
                                    wrapMode, pData),
                  generateMips(generateMips)
            {
            }
        };

        inline operator GLuint() const
        {
            return mTextureId;
        }

        OpenGLTexture(const Args &rArgs);
        OpenGLTexture(const OpenGLTexture &rOther) = delete;
        OpenGLTexture(const OpenGLTexture &&rOther) = delete;
        virtual ~OpenGLTexture();

    private:
        GLuint mTextureId{~0u};

        OpenGLTexture operator=(const OpenGLTexture &rOther) = delete;

        friend class OpenGLGraphicsSystem;
    };
}

#endif

#endif