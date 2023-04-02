#ifndef FASTCG_GRAPHICS_ENUMS_H
#define FASTCG_GRAPHICS_ENUMS_H

#include <FastCG/FastCG.h>
#include <FastCG/Enums.h>

#include <cstdint>
#include <cassert>

namespace FastCG
{
    struct BitsPerPixel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    FASTCG_DECLARE_ENUM(BufferType, uint8_t, UNIFORM, SHADER_STORAGE, VERTEX_ATTRIBUTE, INDICES);
    FASTCG_DECLARE_ENUM(BufferUsage, uint8_t, STATIC, DYNAMIC, STREAM);
    FASTCG_DECLARE_ENUM(VertexDataType, uint8_t, NONE, FLOAT, UNSIGNED_BYTE);
    FASTCG_DECLARE_ENUM(ShaderType, uint8_t, VERTEX, FRAGMENT);
    FASTCG_DECLARE_ENUM(TextureType, uint8_t, TEXTURE_2D);
    FASTCG_DECLARE_ENUM(TextureWrapMode, uint8_t, CLAMP, REPEAT);
    FASTCG_DECLARE_ENUM(TextureFilter, uint8_t, POINT_FILTER, LINEAR_FILTER);
    FASTCG_DECLARE_ENUM(TextureFormat, uint8_t, R, RG, RGB, RGBA, BGR, BGRA, DEPTH_STENCIL, DEPTH);
    FASTCG_DECLARE_ENUM(TextureDataType, uint8_t, FLOAT, UNSIGNED_CHAR, UNSIGNED_INT);
    FASTCG_DECLARE_ENUM(PrimitiveType, uint8_t, TRIANGLES);
    FASTCG_DECLARE_ENUM(BlendFunc, uint8_t, NONE, ADD);
    FASTCG_DECLARE_ENUM(BlendFactor, uint8_t, ZERO, ONE, SRC_COLOR, DST_COLOR, SRC_ALPHA, DST_ALPHA, ONE_MINUS_SRC_COLOR, ONE_MINUS_SRC_ALPHA);
    FASTCG_DECLARE_ENUM(Face, uint8_t, NONE, FRONT, BACK, FRONT_AND_BACK);
    FASTCG_DECLARE_ENUM(CompareOp, uint8_t, NEVER, LESS, LEQUAL, GREATER, GEQUAL, EQUAL, NOT_EQUAL, ALWAYS);
    FASTCG_DECLARE_ENUM(StencilOp, uint8_t, KEEP, ZERO, REPLACE, INVERT, INCREMENT_AND_CLAMP, INCREMENT_AND_WRAP, DECREMENT_AND_CLAMP, DECREMENT_AND_WRAP);

}

#endif