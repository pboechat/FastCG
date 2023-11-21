#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/BaseTexture.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLTexture : public BaseTexture
    {
    public:
        struct Args : BaseTexture::Args
        {
            bool generateMips;

            Args(const std::string &rName = "",
                 uint32_t width = 1,
                 uint32_t height = 1,
                 TextureType type = TextureType::TEXTURE_2D,
                 TextureUsageFlags usage = TextureUsageFlagBit::SAMPLED,
                 TextureFormat format = TextureFormat::RGBA,
                 BitsPerChannel bitsPerChannel = {8, 8, 8, 8},
                 TextureDataType dataType = TextureDataType::UNSIGNED_CHAR,
                 TextureFilter filter = TextureFilter::LINEAR_FILTER,
                 TextureWrapMode wrapMode = TextureWrapMode::CLAMP,
                 const uint8_t *pData = nullptr,
                 bool generateMips = false) : BaseTexture::Args(rName,
                                                                width,
                                                                height,
                                                                type,
                                                                usage,
                                                                format,
                                                                bitsPerChannel,
                                                                dataType,
                                                                filter,
                                                                wrapMode,
                                                                pData),
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