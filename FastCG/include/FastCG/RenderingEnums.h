#ifndef FASTCG_RENDERING_ENUMS_H
#define FASTCG_RENDERING_ENUMS_H

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
        INDICES

    };

    inline const char *GetBufferTypeString(BufferType bufferType)
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

    enum class BufferUsage : uint8_t
    {
        STATIC = 0,
        DYNAMIC,
        STREAM

    };

    inline const char *GetBufferUsageString(BufferUsage bufferUsage)
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

    enum class VertexDataType : uint8_t
    {
        NONE = 0,
        FLOAT,
        UNSIGNED_BYTE

    };

    inline const char *GetVertexDataTypeString(VertexDataType vertexDataType)
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

    enum class ShaderType : uint8_t
    {
        VERTEX = 0,
        FRAGMENT,
        MAX

    };

    inline const char *GetShaderTypeString(ShaderType shaderType)
    {
        switch (shaderType)
        {
            CASE_RETURN_STRING(ShaderType::VERTEX);
            CASE_RETURN_STRING(ShaderType::FRAGMENT);
            CASE_RETURN_STRING(ShaderType::MAX);
        default:
            assert(false);
            return nullptr;
        }
    }

    enum class TextureType : uint8_t
    {
        TEXTURE_2D

    };

    inline const char *GetTextureTypeString(TextureType textureType)
    {
        switch (textureType)
        {
            CASE_RETURN_STRING(TextureType::TEXTURE_2D);
        default:
            assert(false);
            return nullptr;
        }
    }

    enum class TextureWrapMode : uint8_t
    {
        CLAMP = 0,
        REPEAT

    };

    inline const char *GetTextureWrapModeString(TextureWrapMode texureWrapMode)
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

    enum class TextureFilter : uint8_t
    {
        POINT_FILTER = 0,
        LINEAR_FILTER

    };

    inline const char *GetTextureFilterString(TextureFilter textureFilter)
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

    enum class TextureFormat : uint8_t
    {
        R = 0,
        RG,
        RGB,
        RGBA,
        BGR,
        BGRA,
        DEPTH_STENCIL,
        DEPTH

    };

    inline const char *GetTextureFormatString(TextureFormat textureFormat)
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

    enum class TextureDataType : uint8_t
    {
        FLOAT = 0,
        UNSIGNED_CHAR,
        UNSIGNED_INT

    };

    inline const char *GetTextureDataTypeString(TextureDataType textureDataType)
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
        ONE_MINUS_SRC_COLOR,
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