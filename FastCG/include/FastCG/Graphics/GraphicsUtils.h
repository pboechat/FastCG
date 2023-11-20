#ifndef FASTCG_GRAPHICS_UTILS_H
#define FASTCG_GRAPHICS_UTILS_H

#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Enums.h>

#include <cstdint>
#include <cassert>

namespace FastCG
{
    struct BitsPerChannel
    {
        union
        {
            uint8_t r;
            uint8_t depth;
        };
        union
        {
            uint8_t g;
            uint8_t stencil;
        };
        uint8_t b;
        uint8_t a;
    };

    FASTCG_DECLARE_FLAGS(BufferUsage, uint8_t, UNIFORM, SHADER_STORAGE, VERTEX_BUFFER, INDEX_BUFFER, DYNAMIC);
    FASTCG_DECLARE_SCOPED_ENUM(VertexDataType, uint8_t, NONE, FLOAT, UNSIGNED_BYTE);
    FASTCG_DECLARE_SCOPED_ENUM(ShaderType, uint8_t, VERTEX, FRAGMENT);
    FASTCG_DECLARE_SCOPED_ENUM(TextureType, uint8_t, TEXTURE_2D);
    FASTCG_DECLARE_FLAGS(TextureUsage, uint8_t, SAMPLED, RENDER_TARGET, PRESENT);
    FASTCG_DECLARE_SCOPED_ENUM(TextureWrapMode, uint8_t, CLAMP, REPEAT);
    FASTCG_DECLARE_SCOPED_ENUM(TextureFilter, uint8_t, POINT_FILTER, LINEAR_FILTER);
    FASTCG_DECLARE_SCOPED_ENUM(TextureFormat, uint8_t, R, RG, RGB, RGBA, BGR, BGRA, DEPTH_STENCIL, DEPTH);
    FASTCG_DECLARE_SCOPED_ENUM(TextureDataType, uint8_t, FLOAT, UNSIGNED_CHAR, UNSIGNED_INT);
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

    inline bool IsDepthFormat(TextureFormat textureFormat)
    {
        return textureFormat == TextureFormat::DEPTH || textureFormat == TextureFormat::DEPTH_STENCIL;
    }

    inline bool HasStencil(TextureFormat textureFormat)
    {
        return textureFormat == TextureFormat::DEPTH_STENCIL;
    }

}

#endif