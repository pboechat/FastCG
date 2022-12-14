#ifndef FASTCG_RENDERING_ENUMS_H
#define FASTCG_RENDERING_ENUMS_H

#include <cstdint>

namespace FastCG
{
    enum class PrimitiveType : uint8_t
    {
        TRIANGLES = 1,

    };

    enum class BlendFunc : uint8_t
    {
        NONE = 0,
        ADD,

    };

    enum class BlendFactor : uint8_t
    {
        ZERO = 0,
        ONE,
        SRC_COLOR,
        DST_COLOR,
        SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_SRC_ALPHA,

    };

    enum class Face : uint8_t
    {
        NONE = 0,
        FRONT,
        BACK,
        FRONT_AND_BACK,
        MAX

    };

    enum class CompareOp : uint8_t
    {
        NEVER = 0,
        LESS,
        LEQUAL,
        GREATER,
        GEQUAL,
        EQUAL,
        NOT_EQUAL,
        ALWAYS,

    };

    enum class StencilOp : uint8_t
    {
        KEEP = 0,
        ZERO,
        REPLACE,
        INVERT,
        INCREMENT_AND_CLAMP,
        INCREMENT_AND_WRAP,
        DECREMENT_AND_CLAMP,
        DECREMENT_AND_WRAP,

    };
}

#endif