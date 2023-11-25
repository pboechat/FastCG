#ifndef FASTCG_OPENGL_UTILS_H
#define FASTCG_OPENGL_UTILS_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Exception.h>

#ifdef CASE_RETURN
#undef CASE_RETURN
#endif
#define CASE_RETURN(x, y) \
    case x:               \
        return y

#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str) \
    case str:                   \
        return #str

namespace FastCG
{
    inline GLenum GetOpenGLTarget(BufferUsageFlags usage)
    {
        if ((usage & BufferUsageFlagBit::UNIFORM) != 0)
        {
            return GL_UNIFORM_BUFFER;
        }
        else if ((usage & BufferUsageFlagBit::SHADER_STORAGE) != 0)
        {
            return GL_SHADER_STORAGE_BUFFER;
        }
        else if ((usage & BufferUsageFlagBit::VERTEX_BUFFER) != 0)
        {
            return GL_ARRAY_BUFFER;
        }
        else if ((usage & BufferUsageFlagBit::INDEX_BUFFER) != 0)
        {
            return GL_ELEMENT_ARRAY_BUFFER;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL target (usage: %d)", (int)usage);
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLUsageHint(BufferUsageFlags usage)
    {
        if ((usage & BufferUsageFlagBit::DYNAMIC) != 0)
        {
            return GL_DYNAMIC_DRAW;
        }
        else
        {
            return GL_STATIC_DRAW;
        }
    }

    inline GLenum GetOpenGLType(VertexDataType type)
    {
        switch (type)
        {
        case VertexDataType::FLOAT:
            return GL_FLOAT;
        case VertexDataType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL type (type: %s)", GetVertexDataTypeString(type));
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLStencilFunc(StencilOp stencilFunc)
    {
        switch (stencilFunc)
        {
        case StencilOp::KEEP:
            return GL_KEEP;
        case StencilOp::ZERO:
            return GL_ZERO;
        case StencilOp::REPLACE:
            return GL_REPLACE;
        case StencilOp::INVERT:
            return GL_INVERT;
        case StencilOp::INCREMENT_AND_CLAMP:
            return GL_INCR;
        case StencilOp::INCREMENT_AND_WRAP:
            return GL_INCR_WRAP;
        case StencilOp::DECREMENT_AND_CLAMP:
            return GL_DECR;
        case StencilOp::DECREMENT_AND_WRAP:
            return GL_DECR_WRAP;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL stencil func (stencilFunc: %s)", GetStencilOpString(stencilFunc));
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLFace(Face face)
    {
        switch (face)
        {
        case Face::BACK:
            return GL_BACK;
        case Face::FRONT:
            return GL_FRONT;
        case Face::FRONT_AND_BACK:
            return GL_FRONT_AND_BACK;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL face (face: %s)", GetFaceString(face));
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLCompareOp(CompareOp compareOp)
    {
        switch (compareOp)
        {
        case CompareOp::NEVER:
            return GL_NEVER;
        case CompareOp::LESS:
            return GL_LESS;
        case CompareOp::LEQUAL:
            return GL_LEQUAL;
        case CompareOp::GREATER:
            return GL_GREATER;
        case CompareOp::GEQUAL:
            return GL_GEQUAL;
        case CompareOp::EQUAL:
            return GL_EQUAL;
        case CompareOp::NOT_EQUAL:
            return GL_NOTEQUAL;
        case CompareOp::ALWAYS:
            return GL_ALWAYS;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL compare op (compareOp: %s)", GetCompareOpString(compareOp));
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLBlendFunc(BlendFunc blendFunc)
    {
        switch (blendFunc)
        {
        case BlendFunc::ADD:
            return GL_FUNC_ADD;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL blend func (blendFunc: %s)", GetBlendFuncString(blendFunc));
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLBlendFactor(BlendFactor blendFactor)
    {
        switch (blendFactor)
        {
        case BlendFactor::ZERO:
            return GL_ZERO;
        case BlendFactor::ONE:
            return GL_ONE;
        case BlendFactor::SRC_COLOR:
            return GL_SRC_COLOR;
        case BlendFactor::DST_COLOR:
            return GL_DST_COLOR;
        case BlendFactor::SRC_ALPHA:
            return GL_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:
            return GL_DST_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL blend factor (blendFactor: %s)", GetBlendFactorString(blendFactor));
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLPrimitiveType(PrimitiveType primitiveType)
    {
        switch (primitiveType)
        {
        case PrimitiveType::TRIANGLES:
            return GL_TRIANGLES;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL primitive type (primitiveType: %s)", GetPrimitiveTypeString(primitiveType));
            return (GLenum)0;
        }
    }

    inline const char *GetOpenGLShaderTypeString(GLenum shaderType)
    {
        switch (shaderType)
        {
            CASE_RETURN_STRING(GL_VERTEX_SHADER);
            CASE_RETURN_STRING(GL_FRAGMENT_SHADER);
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL shader type string (shaderType: %d)", (int)shaderType);
            return nullptr;
        }
    }

    inline GLenum GetOpenGLShaderType(ShaderType shaderType)
    {
        switch (shaderType)
        {
        case ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL shader type (shaderType: %s)", GetShaderTypeString(shaderType));
            return 0;
        }
    }

    inline GLenum GetOpenGLTarget(TextureType type)
    {
        switch (type)
        {
#if defined GL_TEXTURE_1D
        case TextureType::TEXTURE_1D:
            return GL_TEXTURE_1D;
#endif
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
        case TextureType::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL target (type: %s)", GetTextureTypeString(type));
            return 0;
        }
    }

    inline GLint GetOpenGLFilter(TextureFilter filter)
    {
        switch (filter)
        {
        case TextureFilter::POINT_FILTER:
            return GL_NEAREST;
        case TextureFilter::LINEAR_FILTER:
            return GL_LINEAR;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL filter (filter: %s)", GetTextureFilterString(filter));
            return 0;
        }
    }

    inline GLint GetOpenGLWrapMode(TextureWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case TextureWrapMode::CLAMP:
            return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::REPEAT:
            return GL_REPEAT;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL wrap mode (wrapMode: %s)", GetTextureWrapModeString(wrapMode));
            return 0;
        }
    }

    inline GLint GetOpenGLInternalFormat(TextureFormat format)
    {
        switch (format)
        {
            CASE_RETURN(TextureFormat::R32_SFLOAT, GL_R32F);
            CASE_RETURN(TextureFormat::R16_SFLOAT, GL_R16F);
#ifdef GL_R16
            CASE_RETURN(TextureFormat::R16_UNORM, GL_R16);
#endif
            CASE_RETURN(TextureFormat::R8_UNORM, GL_R8);
            CASE_RETURN(TextureFormat::R32G32_SFLOAT, GL_RG32F);
            CASE_RETURN(TextureFormat::R16G16_SFLOAT, GL_RG16F);
#ifdef GL_RG16
            CASE_RETURN(TextureFormat::R16G16_UNORM, GL_RG16);
#endif
            CASE_RETURN(TextureFormat::R8G8_UNORM, GL_RG8);
            CASE_RETURN(TextureFormat::R32G32B32_SFLOAT, GL_RGB32F);
#ifdef GL_RGB16
            CASE_RETURN(TextureFormat::R16G16B16_UNORM, GL_RGB16);
#endif
            CASE_RETURN(TextureFormat::R16G16B16_SFLOAT, GL_RGB16F);
            CASE_RETURN(TextureFormat::R8G8B8_UNORM, GL_RGB8);
#ifdef GL_BGR
            CASE_RETURN(TextureFormat::B8G8R8_UNORM, GL_BGR);
#endif
            CASE_RETURN(TextureFormat::B10G11R11_UFLOAT_PACK32, GL_R11F_G11F_B10F);
            CASE_RETURN(TextureFormat::R32G32B32A32_SFLOAT, GL_RGBA32F);
#ifdef GL_RGBA16
            CASE_RETURN(TextureFormat::R16G16B16A16_UNORM, GL_RGBA16);
#endif
            CASE_RETURN(TextureFormat::A2R10G10B10_UNORM_PACK32, GL_RGB10_A2);
            CASE_RETURN(TextureFormat::R8G8B8A8_UNORM, GL_RGBA8);
#ifdef GL_BGRA
            CASE_RETURN(TextureFormat::B8G8R8A8_UNORM, GL_BGRA);
#endif
            CASE_RETURN(TextureFormat::D24_UNORM_S8_UINT, GL_DEPTH24_STENCIL8);
            CASE_RETURN(TextureFormat::D32_SFLOAT, GL_DEPTH_COMPONENT32F);
            CASE_RETURN(TextureFormat::X8_D24_UNORM_PACK32, GL_DEPTH_COMPONENT24);
            CASE_RETURN(TextureFormat::D16_UNORM, GL_DEPTH_COMPONENT16);
#ifdef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
            CASE_RETURN(TextureFormat::BC1_RGB_UNORM_BLOCK, GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
            CASE_RETURN(TextureFormat::BC1_RGBA_UNORM_BLOCK, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
            CASE_RETURN(TextureFormat::BC2_UNORM_BLOCK, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
            CASE_RETURN(TextureFormat::BC3_UNORM_BLOCK, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
#endif
#ifdef GL_COMPRESSED_RED_RGTC1
            CASE_RETURN(TextureFormat::BC4_UNORM_BLOCK, GL_COMPRESSED_RED_RGTC1);
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1
            CASE_RETURN(TextureFormat::BC4_SNORM_BLOCK, GL_COMPRESSED_SIGNED_RED_RGTC1);
#endif
#ifdef GL_COMPRESSED_RG_RGTC2
            CASE_RETURN(TextureFormat::BC5_UNORM_BLOCK, GL_COMPRESSED_RG_RGTC2);
#endif
#ifdef GL_COMPRESSED_SIGNED_RG_RGTC2
            CASE_RETURN(TextureFormat::BC5_SNORM_BLOCK, GL_COMPRESSED_SIGNED_RG_RGTC2);
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
            CASE_RETURN(TextureFormat::BC6H_UFLOAT_BLOCK, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB);
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
            CASE_RETURN(TextureFormat::BC6H_SFLOAT_BLOCK, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB);
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
            CASE_RETURN(TextureFormat::BC7_UNORM_BLOCK, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB);
#endif
            CASE_RETURN(TextureFormat::ASTC_4x4_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_4x4_KHR);
            CASE_RETURN(TextureFormat::ASTC_5x4_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_5x4_KHR);
            CASE_RETURN(TextureFormat::ASTC_5x5_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_5x5_KHR);
            CASE_RETURN(TextureFormat::ASTC_6x5_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_6x5_KHR);
            CASE_RETURN(TextureFormat::ASTC_6x6_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_6x6_KHR);
            CASE_RETURN(TextureFormat::ASTC_8x5_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_8x5_KHR);
            CASE_RETURN(TextureFormat::ASTC_8x6_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_8x6_KHR);
            CASE_RETURN(TextureFormat::ASTC_8x8_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_8x8_KHR);
            CASE_RETURN(TextureFormat::ASTC_10x5_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_10x5_KHR);
            CASE_RETURN(TextureFormat::ASTC_10x6_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_10x6_KHR);
            CASE_RETURN(TextureFormat::ASTC_10x8_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_10x8_KHR);
            CASE_RETURN(TextureFormat::ASTC_10x10_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_10x10_KHR);
            CASE_RETURN(TextureFormat::ASTC_12x10_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_12x10_KHR);
            CASE_RETURN(TextureFormat::ASTC_12x12_UNORM_BLOCK, GL_COMPRESSED_RGBA_ASTC_12x12_KHR);
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL internal format (format: %s)", GetTextureFormatString(format));
            return 0;
        }
    }

    inline GLenum GetOpenGLFormat(TextureFormat format)
    {
        switch (format)
        {
            CASE_RETURN(TextureFormat::R32_SFLOAT, GL_RED);
            CASE_RETURN(TextureFormat::R16_SFLOAT, GL_RED);
            CASE_RETURN(TextureFormat::R16_UNORM, GL_RED);
            CASE_RETURN(TextureFormat::R8_UNORM, GL_RED);
            CASE_RETURN(TextureFormat::R32G32_SFLOAT, GL_RG);
            CASE_RETURN(TextureFormat::R16G16_SFLOAT, GL_RG);
            CASE_RETURN(TextureFormat::R16G16_UNORM, GL_RG);
            CASE_RETURN(TextureFormat::R8G8_UNORM, GL_RG);
            CASE_RETURN(TextureFormat::R32G32B32_SFLOAT, GL_RGB);
            CASE_RETURN(TextureFormat::R16G16B16_UNORM, GL_RGB);
            CASE_RETURN(TextureFormat::R16G16B16_SFLOAT, GL_RGB);
            CASE_RETURN(TextureFormat::R8G8B8_UNORM, GL_RGB);
#ifdef GL_BGR
            CASE_RETURN(TextureFormat::B8G8R8_UNORM, GL_BGR);
#endif
            CASE_RETURN(TextureFormat::B10G11R11_UFLOAT_PACK32, GL_RGB);
            CASE_RETURN(TextureFormat::R32G32B32A32_SFLOAT, GL_RGBA);
            CASE_RETURN(TextureFormat::R16G16B16A16_UNORM, GL_RGBA);
            CASE_RETURN(TextureFormat::A2R10G10B10_UNORM_PACK32, GL_RGBA);
            CASE_RETURN(TextureFormat::R8G8B8A8_UNORM, GL_RGBA);
#ifdef GL_BGRA
            CASE_RETURN(TextureFormat::B8G8R8A8_UNORM, GL_BGRA);
#endif
            CASE_RETURN(TextureFormat::D24_UNORM_S8_UINT, GL_DEPTH_STENCIL);
            CASE_RETURN(TextureFormat::D32_SFLOAT, GL_DEPTH_COMPONENT);
            CASE_RETURN(TextureFormat::X8_D24_UNORM_PACK32, GL_DEPTH_COMPONENT);
            CASE_RETURN(TextureFormat::D16_UNORM, GL_DEPTH_COMPONENT);
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL format (format: %s)", GetTextureFormatString(format));
            return 0;
        }
    }

    inline GLenum GetOpenGLDataType(TextureFormat format)
    {
        switch (format)
        {
            CASE_RETURN(TextureFormat::R32_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R16_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R16_UNORM, GL_UNSIGNED_SHORT);
            CASE_RETURN(TextureFormat::R8_UNORM, GL_UNSIGNED_BYTE);
            CASE_RETURN(TextureFormat::R32G32_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R16G16_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R16G16_UNORM, GL_UNSIGNED_SHORT);
            CASE_RETURN(TextureFormat::R8G8_UNORM, GL_UNSIGNED_BYTE);
            CASE_RETURN(TextureFormat::R32G32B32_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R16G16B16_UNORM, GL_UNSIGNED_SHORT);
            CASE_RETURN(TextureFormat::R16G16B16_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R8G8B8_UNORM, GL_UNSIGNED_BYTE);
#ifdef GL_BGR
            CASE_RETURN(TextureFormat::B8G8R8_UNORM, GL_UNSIGNED_BYTE);
#endif
            CASE_RETURN(TextureFormat::B10G11R11_UFLOAT_PACK32, GL_UNSIGNED_INT_10F_11F_11F_REV);
            CASE_RETURN(TextureFormat::R32G32B32A32_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::R16G16B16A16_UNORM, GL_UNSIGNED_SHORT);
            CASE_RETURN(TextureFormat::A2R10G10B10_UNORM_PACK32, GL_UNSIGNED_INT_2_10_10_10_REV);
            CASE_RETURN(TextureFormat::R8G8B8A8_UNORM, GL_UNSIGNED_BYTE);
#ifdef GL_BGRA
            CASE_RETURN(TextureFormat::B8G8R8A8_UNORM, GL_UNSIGNED_BYTE);
#endif
            CASE_RETURN(TextureFormat::D24_UNORM_S8_UINT, GL_UNSIGNED_INT_24_8);
            CASE_RETURN(TextureFormat::D32_SFLOAT, GL_FLOAT);
            CASE_RETURN(TextureFormat::X8_D24_UNORM_PACK32, GL_UNSIGNED_INT);
            CASE_RETURN(TextureFormat::D16_UNORM, GL_FLOAT);
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a GL data type (format: %s)", GetTextureFormatString(format));
            return 0;
        }
    }

    inline const char *GetOpenGLDebugOutputMessageSourceString(GLenum source)
    {
        switch (source)
        {
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_API);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_SHADER_COMPILER);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_APPLICATION);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_OTHER);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Couldn't get a GL debug output message source string (source: %d)", (int)source);
            return nullptr;
        }
    }

    inline const char *GetOpenGLDebugOutputMessageTypeString(GLenum type)
    {
        switch (type)
        {
            CASE_RETURN_STRING(GL_DEBUG_TYPE_ERROR);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_PERFORMANCE);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_PORTABILITY);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_OTHER);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_MARKER);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_PUSH_GROUP);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_POP_GROUP);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Couldn't get a GL debug output message type string (type: %d)", (int)type);
            return nullptr;
        }
    }

    inline const char *GetOpenGLDebugOutputMessageSeverity(GLenum severity)
    {
        switch (severity)
        {
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_HIGH);
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_MEDIUM);
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_LOW);
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_NOTIFICATION);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Couldn't get a GL debug output message severity (severity: %d)", (int)severity);
            return nullptr;
        }
    }
}

#undef CASE_RETURN
#undef CASE_RETURN_STRING

#endif

#endif