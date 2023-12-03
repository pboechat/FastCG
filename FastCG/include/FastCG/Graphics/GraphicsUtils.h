#ifndef FASTCG_GRAPHICS_UTILS_H
#define FASTCG_GRAPHICS_UTILS_H

#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Enums.h>

#include <cmath>
#include <cstdint>
#include <cassert>

namespace FastCG
{
    FASTCG_DECLARE_FLAGS(BufferUsage, uint8_t, UNIFORM, SHADER_STORAGE, VERTEX_BUFFER, INDEX_BUFFER, DYNAMIC);
    FASTCG_DECLARE_SCOPED_ENUM(VertexDataType, uint8_t, NONE, FLOAT, UNSIGNED_BYTE);
    FASTCG_DECLARE_SCOPED_ENUM(ShaderType, uint8_t, VERTEX, FRAGMENT);
    FASTCG_DECLARE_SCOPED_ENUM(TextureType, uint8_t, TEXTURE_1D, TEXTURE_2D, TEXTURE_3D, TEXTURE_CUBE_MAP, TEXTURE_2D_ARRAY);
    FASTCG_DECLARE_FLAGS(TextureUsage, uint8_t, SAMPLED, RENDER_TARGET, PRESENT);
    FASTCG_DECLARE_SCOPED_ENUM(TextureWrapMode, uint8_t, CLAMP, REPEAT);
    FASTCG_DECLARE_SCOPED_ENUM(TextureFilter, uint8_t, POINT_FILTER, LINEAR_FILTER);
    FASTCG_DECLARE_SCOPED_ENUM(TextureFormat,
                               uint8_t,
                               R32_SFLOAT,
                               R16_SFLOAT,
                               R16_UNORM,
                               R8_UNORM,
                               R32G32_SFLOAT,
                               R16G16_SFLOAT,
                               R16G16_UNORM,
                               R8G8_UNORM,
                               R32G32B32_SFLOAT,
                               R16G16B16_UNORM,
                               R16G16B16_SFLOAT,
                               R8G8B8_UNORM,
                               B8G8R8_UNORM,
                               B10G11R11_UFLOAT_PACK32,
                               R32G32B32A32_SFLOAT,
                               R16G16B16A16_UNORM,
                               A2R10G10B10_UNORM_PACK32,
                               R8G8B8A8_UNORM,
                               B8G8R8A8_UNORM,
                               D24_UNORM_S8_UINT,
                               D32_SFLOAT,
                               X8_D24_UNORM_PACK32,
                               D16_UNORM,
                               BC1_RGB_UNORM_BLOCK,
                               BC1_RGBA_UNORM_BLOCK,
                               BC2_UNORM_BLOCK,
                               BC3_UNORM_BLOCK,
                               BC4_UNORM_BLOCK,
                               BC4_SNORM_BLOCK,
                               BC5_UNORM_BLOCK,
                               BC5_SNORM_BLOCK,
                               BC6H_UFLOAT_BLOCK,
                               BC6H_SFLOAT_BLOCK,
                               BC7_UNORM_BLOCK,
                               ASTC_4x4_UNORM_BLOCK,
                               ASTC_5x4_UNORM_BLOCK,
                               ASTC_5x5_UNORM_BLOCK,
                               ASTC_6x5_UNORM_BLOCK,
                               ASTC_6x6_UNORM_BLOCK,
                               ASTC_8x5_UNORM_BLOCK,
                               ASTC_8x6_UNORM_BLOCK,
                               ASTC_8x8_UNORM_BLOCK,
                               ASTC_10x5_UNORM_BLOCK,
                               ASTC_10x6_UNORM_BLOCK,
                               ASTC_10x8_UNORM_BLOCK,
                               ASTC_10x10_UNORM_BLOCK,
                               ASTC_12x10_UNORM_BLOCK,
                               ASTC_12x12_UNORM_BLOCK);
    FASTCG_DECLARE_SCOPED_ENUM(PrimitiveType, uint8_t, TRIANGLES);
    FASTCG_DECLARE_SCOPED_ENUM(BlendFunc, uint8_t, NONE, ADD);
    FASTCG_DECLARE_SCOPED_ENUM(BlendFactor, uint8_t, ZERO, ONE, SRC_COLOR, DST_COLOR, SRC_ALPHA, DST_ALPHA, ONE_MINUS_SRC_COLOR, ONE_MINUS_SRC_ALPHA);
    FASTCG_DECLARE_SCOPED_ENUM(Face, uint8_t, NONE, FRONT, BACK, FRONT_AND_BACK);
    FASTCG_DECLARE_SCOPED_ENUM(CompareOp, uint8_t, LEQUAL, LESS, GEQUAL, GREATER, EQUAL, NOT_EQUAL, ALWAYS, NEVER);
    FASTCG_DECLARE_SCOPED_ENUM(StencilOp, uint8_t, KEEP, ZERO, REPLACE, INVERT, INCREMENT_AND_CLAMP, INCREMENT_AND_WRAP, DECREMENT_AND_CLAMP, DECREMENT_AND_WRAP);
    FASTCG_DECLARE_SCOPED_ENUM(FogMode, uint8_t, NONE, LINEAR, EXP, EXP2);

    struct BlendState
    {
        BlendFunc colorOp;
        BlendFactor srcColorFactor;
        BlendFactor dstColorFactor;
        BlendFunc alphaOp;
        BlendFactor srcAlphaFactor;
        BlendFactor dstAlphaFactor;
    };

    struct StencilState
    {
        CompareOp compareOp;
        StencilOp passOp;
        StencilOp stencilFailOp;
        StencilOp depthFailOp;
        int32_t reference;
        uint32_t compareMask;
        uint32_t writeMask;
    };

    struct VertexBindingDescriptor
    {
        uint32_t binding;
        uint32_t size;
        VertexDataType type;
        bool normalized;
        uint32_t stride;
        uint32_t offset;

        inline bool IsValid() const
        {
            return size > 0 && type != VertexDataType::NONE;
        }
    };

    inline bool IsDepthFormat(TextureFormat format)
    {
        return (uint8_t)format >= (uint8_t)TextureFormat::D24_UNORM_S8_UINT &&
               (uint8_t)format <= (uint8_t)TextureFormat::D16_UNORM;
    }

    inline bool IsColorFormat(TextureFormat format)
    {
        return !IsDepthFormat(format);
    }

    inline bool HasStencil(TextureFormat format)
    {
        return format == TextureFormat::D24_UNORM_S8_UINT;
    }

    inline bool IsASTCCompressed(TextureFormat format)
    {
        return (uint8_t)format >= (uint8_t)TextureFormat::ASTC_4x4_UNORM_BLOCK &&
               (uint8_t)format <= (uint8_t)TextureFormat::ASTC_12x12_UNORM_BLOCK;
    }

    inline bool IsBCCompressed(TextureFormat format)
    {
        return (uint8_t)format >= (uint8_t)TextureFormat::BC1_RGB_UNORM_BLOCK &&
               (uint8_t)format <= (uint8_t)TextureFormat::BC7_UNORM_BLOCK;
    }

    inline bool IsCompressed(TextureFormat format)
    {
        return IsASTCCompressed(format) || IsBCCompressed(format);
    }

    inline uint32_t GetBlockWidth(TextureFormat format)
    {
        if (IsBCCompressed(format))
        {
            return 4;
        }
        else if (format == TextureFormat::ASTC_4x4_UNORM_BLOCK)
        {
            return 4;
        }
        else if (format == TextureFormat::ASTC_5x4_UNORM_BLOCK)
        {
            return 5;
        }
        else if (format == TextureFormat::ASTC_5x5_UNORM_BLOCK)
        {
            return 5;
        }
        else if (format == TextureFormat::ASTC_6x5_UNORM_BLOCK)
        {
            return 6;
        }
        else if (format == TextureFormat::ASTC_6x6_UNORM_BLOCK)
        {
            return 6;
        }
        else if (format == TextureFormat::ASTC_8x5_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::ASTC_8x6_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::ASTC_8x8_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::ASTC_10x5_UNORM_BLOCK)
        {
            return 10;
        }
        else if (format == TextureFormat::ASTC_10x6_UNORM_BLOCK)
        {
            return 10;
        }
        else if (format == TextureFormat::ASTC_10x8_UNORM_BLOCK)
        {
            return 10;
        }
        else if (format == TextureFormat::ASTC_10x10_UNORM_BLOCK)
        {
            return 10;
        }
        else if (format == TextureFormat::ASTC_12x10_UNORM_BLOCK)
        {
            return 12;
        }
        else if (format == TextureFormat::ASTC_12x12_UNORM_BLOCK)
        {
            return 12;
        }
        FASTCG_THROW_EXCEPTION(Exception, "Couldn't get block width (format: %s)", GetTextureFormatString(format));
        return 1;
    }

    inline uint32_t GetBlockHeight(TextureFormat format)
    {
        if (IsBCCompressed(format))
        {
            return 4;
        }
        else if (format == TextureFormat::ASTC_4x4_UNORM_BLOCK)
        {
            return 4;
        }
        else if (format == TextureFormat::ASTC_5x4_UNORM_BLOCK)
        {
            return 4;
        }
        else if (format == TextureFormat::ASTC_5x5_UNORM_BLOCK)
        {
            return 5;
        }
        else if (format == TextureFormat::ASTC_6x5_UNORM_BLOCK)
        {
            return 5;
        }
        else if (format == TextureFormat::ASTC_6x6_UNORM_BLOCK)
        {
            return 6;
        }
        else if (format == TextureFormat::ASTC_8x5_UNORM_BLOCK)
        {
            return 5;
        }
        else if (format == TextureFormat::ASTC_8x6_UNORM_BLOCK)
        {
            return 6;
        }
        else if (format == TextureFormat::ASTC_8x8_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::ASTC_10x5_UNORM_BLOCK)
        {
            return 5;
        }
        else if (format == TextureFormat::ASTC_10x6_UNORM_BLOCK)
        {
            return 6;
        }
        else if (format == TextureFormat::ASTC_10x8_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::ASTC_10x10_UNORM_BLOCK)
        {
            return 10;
        }
        else if (format == TextureFormat::ASTC_12x10_UNORM_BLOCK)
        {
            return 10;
        }
        else if (format == TextureFormat::ASTC_12x12_UNORM_BLOCK)
        {
            return 12;
        }
        FASTCG_THROW_EXCEPTION(Exception, "Couldn't get block height (format: %s)", GetTextureFormatString(format));
        return 0;
    }

    inline uint32_t GetBlockDepth(TextureFormat format)
    {
        // TODO:
        return 1;
    }

    inline uint32_t GetBytesPerBlock(TextureFormat format)
    {
        if (IsASTCCompressed(format))
        {
            return 16;
        }
        else if (format == TextureFormat::BC1_RGB_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::BC1_RGBA_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::BC2_UNORM_BLOCK)
        {
            return 16;
        }
        else if (format == TextureFormat::BC3_UNORM_BLOCK)
        {
            return 16;
        }
        else if (format == TextureFormat::BC4_UNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::BC4_SNORM_BLOCK)
        {
            return 8;
        }
        else if (format == TextureFormat::BC5_SNORM_BLOCK)
        {
            return 16;
        }
        else if (format == TextureFormat::BC6H_UFLOAT_BLOCK)
        {
            return 16;
        }
        else if (format == TextureFormat::BC6H_SFLOAT_BLOCK)
        {
            return 16;
        }
        else if (format == TextureFormat::BC7_UNORM_BLOCK)
        {
            return 16;
        }
        FASTCG_THROW_EXCEPTION(Exception, "Couldn't get bytes per block (format: %s)", GetTextureFormatString(format));
        return 0;
    }

    inline uint32_t GetBytesPerTexel(TextureFormat format)
    {
#ifdef CASE_RETURN
#undef CASE_RETURN
#endif
#define CASE_RETURN(x, y)  \
    case TextureFormat::x: \
        return y
        switch (format)
        {
            CASE_RETURN(R32_SFLOAT, 4);
            CASE_RETURN(R16_SFLOAT, 2);
            CASE_RETURN(R16_UNORM, 2);
            CASE_RETURN(R8_UNORM, 1);
            CASE_RETURN(R32G32_SFLOAT, 8);
            CASE_RETURN(R16G16_SFLOAT, 4);
            CASE_RETURN(R16G16_UNORM, 4);
            CASE_RETURN(R8G8_UNORM, 2);
            CASE_RETURN(R32G32B32_SFLOAT, 12);
            CASE_RETURN(R16G16B16_UNORM, 6);
            CASE_RETURN(R16G16B16_SFLOAT, 6);
            CASE_RETURN(R8G8B8_UNORM, 3);
            CASE_RETURN(B8G8R8_UNORM, 3);
            CASE_RETURN(B10G11R11_UFLOAT_PACK32, 4);
            CASE_RETURN(R32G32B32A32_SFLOAT, 16);
            CASE_RETURN(R16G16B16A16_UNORM, 8);
            CASE_RETURN(A2R10G10B10_UNORM_PACK32, 4);
            CASE_RETURN(R8G8B8A8_UNORM, 4);
            CASE_RETURN(B8G8R8A8_UNORM, 4);
            CASE_RETURN(D24_UNORM_S8_UINT, 4);
            CASE_RETURN(D32_SFLOAT, 4);
            CASE_RETURN(X8_D24_UNORM_PACK32, 4);
            CASE_RETURN(D16_UNORM, 2);
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get bytes per texel (format: %s)", GetTextureFormatString(format));
            return 0;
        }
#undef CASE_RETURN
    }

    inline size_t GetTextureDataSize(TextureFormat format, uint32_t width, uint32_t height, uint32_t depth)
    {
        if (IsCompressed(format))
        {
            auto hBlockCount = (uint32_t)std::ceil(width / (float)GetBlockWidth(format));
            auto vBlockCount = (uint32_t)std::ceil(height / (float)GetBlockHeight(format));
            auto dBlockCount = (uint32_t)std::ceil(depth / (float)GetBlockDepth(format));
            auto bytesPerBlock = GetBytesPerBlock(format);
            return hBlockCount * vBlockCount * dBlockCount * bytesPerBlock;
        }
        else
        {
            auto bytesPerTexel = GetBytesPerTexel(format);
            return width * height * depth * bytesPerTexel;
        }
    }

}

#endif