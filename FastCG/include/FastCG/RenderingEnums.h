#ifndef FASTCG_RENDERING_ENUMS_H
#define FASTCG_RENDERING_ENUMS_H

#include <FastCG/FastCG.h>

#include <cstdint>
#include <cassert>

#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str) \
    case str:                   \
        return #str

namespace FastCG
{
    enum class BufferType : uint8_t
    {
        UNIFORM = 0,
        SHADER_STORAGE,
        VERTEX_ATTRIBUTE,
        INDICES,
        MAX

    };

    inline constexpr const char *GetBufferTypeString(BufferType bufferType)
    {
        switch (bufferType)
        {
            CASE_RETURN_STRING(BufferType::UNIFORM);
            CASE_RETURN_STRING(BufferType::SHADER_STORAGE);
            CASE_RETURN_STRING(BufferType::VERTEX_ATTRIBUTE);
            CASE_RETURN_STRING(BufferType::INDICES);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const BUFFER_TYPE_STRINGS[] = {GetBufferTypeString(BufferType::UNIFORM),
                                                         GetBufferTypeString(BufferType::SHADER_STORAGE),
                                                         GetBufferTypeString(BufferType::VERTEX_ATTRIBUTE),
                                                         GetBufferTypeString(BufferType::INDICES)};

    static_assert((size_t)BufferType::MAX == FASTCG_ARRAYSIZE(BUFFER_TYPE_STRINGS),
                  "Missing element in BUFFER_TYPE_STRINGS");

    enum class BufferUsage : uint8_t
    {
        STATIC = 0,
        DYNAMIC,
        STREAM,
        MAX

    };

    inline constexpr const char *GetBufferUsageString(BufferUsage bufferUsage)
    {
        switch (bufferUsage)
        {
            CASE_RETURN_STRING(BufferUsage::STATIC);
            CASE_RETURN_STRING(BufferUsage::DYNAMIC);
            CASE_RETURN_STRING(BufferUsage::STREAM);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const BUFFER_USAGE_STRINGS[] = {GetBufferUsageString(BufferUsage::STATIC),
                                                          GetBufferUsageString(BufferUsage::DYNAMIC),
                                                          GetBufferUsageString(BufferUsage::STREAM)};

    static_assert((size_t)BufferUsage::MAX == FASTCG_ARRAYSIZE(BUFFER_USAGE_STRINGS),
                  "Missing element in BUFFER_USAGE_STRINGS");

    enum class VertexDataType : uint8_t
    {
        NONE = 0,
        FLOAT,
        UNSIGNED_BYTE,
        MAX

    };

    inline constexpr const char *GetVertexDataTypeString(VertexDataType vertexDataType)
    {
        switch (vertexDataType)
        {
            CASE_RETURN_STRING(VertexDataType::NONE);
            CASE_RETURN_STRING(VertexDataType::FLOAT);
            CASE_RETURN_STRING(VertexDataType::UNSIGNED_BYTE);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const VERTEX_DATA_TYPE_STRINGS[] = {GetVertexDataTypeString(VertexDataType::NONE),
                                                              GetVertexDataTypeString(VertexDataType::FLOAT),
                                                              GetVertexDataTypeString(VertexDataType::UNSIGNED_BYTE)};

    static_assert((size_t)VertexDataType::MAX == FASTCG_ARRAYSIZE(VERTEX_DATA_TYPE_STRINGS),
                  "Missing element in VERTEX_DATA_TYPE_STRINGS");

    enum class ShaderType : uint8_t
    {
        VERTEX = 0,
        FRAGMENT,
        MAX

    };

    inline constexpr const char *GetShaderTypeString(ShaderType shaderType)
    {
        switch (shaderType)
        {
            CASE_RETURN_STRING(ShaderType::VERTEX);
            CASE_RETURN_STRING(ShaderType::FRAGMENT);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const SHADER_TYPE_STRINGS[] = {GetShaderTypeString(ShaderType::VERTEX),
                                                         GetShaderTypeString(ShaderType::FRAGMENT)};

    static_assert((size_t)ShaderType::MAX == FASTCG_ARRAYSIZE(SHADER_TYPE_STRINGS),
                  "Missing element in SHADER_TYPE_STRINGS");

    enum class TextureType : uint8_t
    {
        TEXTURE_2D,
        MAX

    };

    inline constexpr const char *GetTextureTypeString(TextureType textureType)
    {
        switch (textureType)
        {
            CASE_RETURN_STRING(TextureType::TEXTURE_2D);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const TEXTURE_TYPE_STRINGS[] = {GetTextureTypeString(TextureType::TEXTURE_2D)};

    static_assert((size_t)TextureType::MAX == FASTCG_ARRAYSIZE(TEXTURE_TYPE_STRINGS),
                  "Missing element in TEXTURE_TYPE_STRINGS");

    enum class TextureWrapMode : uint8_t
    {
        CLAMP = 0,
        REPEAT,
        MAX

    };

    inline constexpr const char *GetTextureWrapModeString(TextureWrapMode texureWrapMode)
    {
        switch (texureWrapMode)
        {
            CASE_RETURN_STRING(TextureWrapMode::CLAMP);
            CASE_RETURN_STRING(TextureWrapMode::REPEAT);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const TEXTURE_WRAP_MODE_STRINGS[] = {GetTextureWrapModeString(TextureWrapMode::CLAMP),
                                                               GetTextureWrapModeString(TextureWrapMode::REPEAT)};

    static_assert((size_t)TextureWrapMode::MAX == FASTCG_ARRAYSIZE(TEXTURE_WRAP_MODE_STRINGS),
                  "Missing element in TEXTURE_WRAP_MODE_STRINGS");

    enum class TextureFilter : uint8_t
    {
        POINT_FILTER = 0,
        LINEAR_FILTER,
        MAX

    };

    inline constexpr const char *GetTextureFilterString(TextureFilter textureFilter)
    {
        switch (textureFilter)
        {
            CASE_RETURN_STRING(TextureFilter::POINT_FILTER);
            CASE_RETURN_STRING(TextureFilter::LINEAR_FILTER);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const TEXTURE_FILTER_STRINGS[] = {GetTextureFilterString(TextureFilter::POINT_FILTER),
                                                            GetTextureFilterString(TextureFilter::LINEAR_FILTER)};

    static_assert((size_t)TextureFilter::MAX == FASTCG_ARRAYSIZE(TEXTURE_FILTER_STRINGS),
                  "Missing element in TEXTURE_FILTER_STRINGS");

    enum class TextureFormat : uint8_t
    {
        R = 0,
        RG,
        RGB,
        RGBA,
        BGR,
        BGRA,
        DEPTH_STENCIL,
        DEPTH,
        MAX

    };

    inline constexpr const char *GetTextureFormatString(TextureFormat textureFormat)
    {
        switch (textureFormat)
        {
            CASE_RETURN_STRING(TextureFormat::R);
            CASE_RETURN_STRING(TextureFormat::RG);
            CASE_RETURN_STRING(TextureFormat::RGB);
            CASE_RETURN_STRING(TextureFormat::RGBA);
            CASE_RETURN_STRING(TextureFormat::BGR);
            CASE_RETURN_STRING(TextureFormat::BGRA);
            CASE_RETURN_STRING(TextureFormat::DEPTH_STENCIL);
            CASE_RETURN_STRING(TextureFormat::DEPTH);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const TEXTURE_FORMAT_STRINGS[] = {GetTextureFormatString(TextureFormat::R),
                                                            GetTextureFormatString(TextureFormat::RG),
                                                            GetTextureFormatString(TextureFormat::RGB),
                                                            GetTextureFormatString(TextureFormat::RGBA),
                                                            GetTextureFormatString(TextureFormat::BGR),
                                                            GetTextureFormatString(TextureFormat::BGRA),
                                                            GetTextureFormatString(TextureFormat::DEPTH_STENCIL),
                                                            GetTextureFormatString(TextureFormat::DEPTH)};

    static_assert((size_t)TextureFormat::MAX == FASTCG_ARRAYSIZE(TEXTURE_FORMAT_STRINGS),
                  "Missing element in TEXTURE_FORMAT_STRINGS");

    enum class TextureDataType : uint8_t
    {
        FLOAT = 0,
        UNSIGNED_CHAR,
        UNSIGNED_INT,
        MAX

    };

    inline constexpr const char *GetTextureDataTypeString(TextureDataType textureDataType)
    {
        switch (textureDataType)
        {
            CASE_RETURN_STRING(TextureDataType::FLOAT);
            CASE_RETURN_STRING(TextureDataType::UNSIGNED_CHAR);
            CASE_RETURN_STRING(TextureDataType::UNSIGNED_INT);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const TEXTURE_DATA_TYPE_STRINGS[] = {GetTextureDataTypeString(TextureDataType::FLOAT),
                                                               GetTextureDataTypeString(TextureDataType::UNSIGNED_CHAR),
                                                               GetTextureDataTypeString(TextureDataType::UNSIGNED_INT)};

    static_assert((size_t)TextureDataType::MAX == FASTCG_ARRAYSIZE(TEXTURE_DATA_TYPE_STRINGS),
                  "Missing element in TEXTURE_DATA_TYPE_STRINGS");

    enum class PrimitiveType : uint8_t
    {
        TRIANGLES = 0,
        MAX

    };

    inline constexpr const char *GetPrimitiveTypeString(PrimitiveType primitiveType)
    {
        switch (primitiveType)
        {
            CASE_RETURN_STRING(PrimitiveType::TRIANGLES);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const PRIMITIVE_TYPE_STRINGS[] = {GetPrimitiveTypeString(PrimitiveType::TRIANGLES)};

    static_assert((size_t)PrimitiveType::MAX == FASTCG_ARRAYSIZE(PRIMITIVE_TYPE_STRINGS),
                  "Missing element in PRIMITIVE_TYPE_STRINGS");

    enum class BlendFunc : uint8_t
    {
        NONE = 0,
        ADD,
        MAX

    };

    inline constexpr const char *GetBlendFuncString(BlendFunc blendFunc)
    {
        switch (blendFunc)
        {
            CASE_RETURN_STRING(BlendFunc::NONE);
            CASE_RETURN_STRING(BlendFunc::ADD);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const BLEND_FUNC_STRINGS[] = {GetBlendFuncString(BlendFunc::NONE),
                                                        GetBlendFuncString(BlendFunc::ADD)};

    static_assert((size_t)BlendFunc::MAX == FASTCG_ARRAYSIZE(BLEND_FUNC_STRINGS),
                  "Missing element in BLEND_FUNC_STRINGS");

    enum class BlendFactor : uint8_t
    {
        ZERO = 0,
        ONE,
        SRC_COLOR,
        DST_COLOR,
        SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_SRC_COLOR,
        ONE_MINUS_SRC_ALPHA,
        MAX

    };

    inline constexpr const char *GetBlendFactorString(BlendFactor blendFactor)
    {
        switch (blendFactor)
        {
            CASE_RETURN_STRING(BlendFactor::ZERO);
            CASE_RETURN_STRING(BlendFactor::ONE);
            CASE_RETURN_STRING(BlendFactor::SRC_COLOR);
            CASE_RETURN_STRING(BlendFactor::DST_COLOR);
            CASE_RETURN_STRING(BlendFactor::SRC_ALPHA);
            CASE_RETURN_STRING(BlendFactor::DST_ALPHA);
            CASE_RETURN_STRING(BlendFactor::ONE_MINUS_SRC_COLOR);
            CASE_RETURN_STRING(BlendFactor::ONE_MINUS_SRC_ALPHA);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const BLEND_FACTOR_STRINGS[] = {GetBlendFactorString(BlendFactor::ZERO),
                                                          GetBlendFactorString(BlendFactor::ONE),
                                                          GetBlendFactorString(BlendFactor::SRC_COLOR),
                                                          GetBlendFactorString(BlendFactor::DST_COLOR),
                                                          GetBlendFactorString(BlendFactor::SRC_ALPHA),
                                                          GetBlendFactorString(BlendFactor::DST_ALPHA),
                                                          GetBlendFactorString(BlendFactor::ONE_MINUS_SRC_COLOR),
                                                          GetBlendFactorString(BlendFactor::ONE_MINUS_SRC_ALPHA)};

    static_assert((size_t)BlendFactor::MAX == FASTCG_ARRAYSIZE(BLEND_FACTOR_STRINGS),
                  "Missing element in BLEND_FACTOR_STRINGS");

    enum class Face : uint8_t
    {
        NONE = 0,
        FRONT,
        BACK,
        FRONT_AND_BACK,
        MAX

    };

    inline constexpr const char *GetFaceString(Face face)
    {
        switch (face)
        {
            CASE_RETURN_STRING(Face::NONE);
            CASE_RETURN_STRING(Face::FRONT);
            CASE_RETURN_STRING(Face::BACK);
            CASE_RETURN_STRING(Face::FRONT_AND_BACK);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const FACE_STRINGS[] = {GetFaceString(Face::NONE),
                                                  GetFaceString(Face::FRONT),
                                                  GetFaceString(Face::BACK),
                                                  GetFaceString(Face::FRONT_AND_BACK)};

    static_assert((size_t)Face::MAX == FASTCG_ARRAYSIZE(FACE_STRINGS),
                  "Missing element in FACE_STRINGS");

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
        MAX

    };

    inline constexpr const char *GetCompareOpString(CompareOp compareOp)
    {
        switch (compareOp)
        {
            CASE_RETURN_STRING(CompareOp::NEVER);
            CASE_RETURN_STRING(CompareOp::LESS);
            CASE_RETURN_STRING(CompareOp::LEQUAL);
            CASE_RETURN_STRING(CompareOp::GREATER);
            CASE_RETURN_STRING(CompareOp::GEQUAL);
            CASE_RETURN_STRING(CompareOp::EQUAL);
            CASE_RETURN_STRING(CompareOp::NOT_EQUAL);
            CASE_RETURN_STRING(CompareOp::ALWAYS);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const COMPARE_OP_STRINGS[] = {GetCompareOpString(CompareOp::NEVER),
                                                        GetCompareOpString(CompareOp::LESS),
                                                        GetCompareOpString(CompareOp::LEQUAL),
                                                        GetCompareOpString(CompareOp::GREATER),
                                                        GetCompareOpString(CompareOp::GEQUAL),
                                                        GetCompareOpString(CompareOp::EQUAL),
                                                        GetCompareOpString(CompareOp::NOT_EQUAL),
                                                        GetCompareOpString(CompareOp::ALWAYS)};

    static_assert((size_t)CompareOp::MAX == FASTCG_ARRAYSIZE(COMPARE_OP_STRINGS),
                  "Missing element in COMPARE_OP_STRINGS");

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
        MAX

    };

    inline constexpr const char *GetStencilOpString(StencilOp stencilOp)
    {
        switch (stencilOp)
        {
            CASE_RETURN_STRING(StencilOp::KEEP);
            CASE_RETURN_STRING(StencilOp::ZERO);
            CASE_RETURN_STRING(StencilOp::REPLACE);
            CASE_RETURN_STRING(StencilOp::INVERT);
            CASE_RETURN_STRING(StencilOp::INCREMENT_AND_CLAMP);
            CASE_RETURN_STRING(StencilOp::INCREMENT_AND_WRAP);
            CASE_RETURN_STRING(StencilOp::DECREMENT_AND_CLAMP);
            CASE_RETURN_STRING(StencilOp::DECREMENT_AND_WRAP);
        default:
            assert(false);
            return nullptr;
        }
    }

    constexpr const char *const STENCIL_OP_STRINGS[] = {GetStencilOpString(StencilOp::KEEP),
                                                        GetStencilOpString(StencilOp::ZERO),
                                                        GetStencilOpString(StencilOp::REPLACE),
                                                        GetStencilOpString(StencilOp::INVERT),
                                                        GetStencilOpString(StencilOp::INCREMENT_AND_CLAMP),
                                                        GetStencilOpString(StencilOp::INCREMENT_AND_WRAP),
                                                        GetStencilOpString(StencilOp::DECREMENT_AND_CLAMP),
                                                        GetStencilOpString(StencilOp::DECREMENT_AND_WRAP)};

    static_assert((size_t)StencilOp::MAX == FASTCG_ARRAYSIZE(STENCIL_OP_STRINGS),
                  "Missing element in STENCIL_OP_STRINGS");

}

#endif