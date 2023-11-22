#ifndef FASTCG_OPENGL_UTILS_H
#define FASTCG_OPENGL_UTILS_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Exception.h>

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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled buffer usage %d", (int)usage);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled vertex data type %d", (int)type);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled stencil func %d", (int)stencilFunc);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled face %d", (int)face);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled compare op %d", (int)compareOp);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled blend func %d", (int)blendFunc);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled blend factor %d", (int)blendFactor);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled primitive type %d", (int)primitiveType);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled shader type %d", (int)shaderType);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled shader type %d", (int)shaderType);
            return 0;
        }
    }

    inline GLenum GetOpenGLTarget(TextureType type)
    {
        switch (type)
        {
        case TextureType::TEXTURE_1D:
            return GL_TEXTURE_1D;
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
        case TextureType::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled texture type %d", (int)type);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled filter %d", (int)filter);
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
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled wrapping mode %d", (int)wrapMode);
            return 0;
        }
    }

    inline GLint GetOpenGLInternalFormat(TextureFormat format, const BitsPerChannel &bitsPerChannel, TextureDataType dataType)
    {
        switch (format)
        {
        case TextureFormat::R:
            if (bitsPerChannel.r == 8 && dataType == TextureDataType::UNSIGNED_CHAR)
            {
                return GL_R8;
            }
#if defined GL_R16
            else if (bitsPerChannel.r == 16 && dataType == TextureDataType::UNSIGNED_SHORT)
            {
                return GL_R16;
            }
#endif
            else if (bitsPerChannel.r == 16 && dataType == TextureDataType::FLOAT)
            {
                return GL_R16F;
            }
            else if (bitsPerChannel.r == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_R32F;
            }
            else if (bitsPerChannel.r == 32 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_R32UI;
            }
            break;
        case TextureFormat::RG:
            if (bitsPerChannel.r == 8 && bitsPerChannel.g == 8 && dataType == TextureDataType::UNSIGNED_CHAR)
            {
                return GL_RG8;
            }
#if defined GL_RG16
            else if (bitsPerChannel.r == 16 && bitsPerChannel.g == 16 && dataType == TextureDataType::UNSIGNED_SHORT)
            {
                return GL_RG16;
            }
#endif
            else if (bitsPerChannel.r == 16 && bitsPerChannel.g == 16 && dataType == TextureDataType::FLOAT)
            {
                return GL_RG16F;
            }
            else if (bitsPerChannel.r == 32 && bitsPerChannel.g == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_RG32F;
            }
            else if (bitsPerChannel.r == 32 && bitsPerChannel.g == 32 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_RG32UI;
            }
            break;
        case TextureFormat::BGR:
            if (bitsPerChannel.r == 8 && bitsPerChannel.g == 8 && bitsPerChannel.b == 8 && dataType == TextureDataType::UNSIGNED_CHAR)
            {
                return GL_BGR;
            }
            break;
        case TextureFormat::RGB:
            if (bitsPerChannel.r == 8 && bitsPerChannel.g == 8 && bitsPerChannel.b == 8 && dataType == TextureDataType::UNSIGNED_CHAR)
            {
                return GL_RGB8;
            }
            else if (bitsPerChannel.r == 11 && bitsPerChannel.g == 11 && bitsPerChannel.b == 10 && dataType == TextureDataType::FLOAT)
            {
                return GL_R11F_G11F_B10F;
            }
            else if (bitsPerChannel.r == 16 && bitsPerChannel.g == 16 && bitsPerChannel.b == 16 && dataType == TextureDataType::FLOAT)
            {
                return GL_RGB16F;
            }
#if defined GL_RGB16
            else if (bitsPerChannel.r == 16 && bitsPerChannel.g == 16 && bitsPerChannel.b == 16 && dataType == TextureDataType::UNSIGNED_SHORT)
            {
                return GL_RGB16;
            }
#endif
            else if (bitsPerChannel.r == 32 && bitsPerChannel.g == 32 && bitsPerChannel.b == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_RGB32F;
            }
            else if (bitsPerChannel.r == 32 && bitsPerChannel.g == 32 && bitsPerChannel.b == 32 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_RGB32UI;
            }
            break;
        case TextureFormat::BGRA:
            if (bitsPerChannel.r == 8 && bitsPerChannel.g == 8 && bitsPerChannel.b == 8 && bitsPerChannel.a == 8 && dataType == TextureDataType::UNSIGNED_CHAR)
            {
                return GL_BGRA;
            }
            break;
        case TextureFormat::RGBA:
            if (bitsPerChannel.r == 8 && bitsPerChannel.g == 8 && bitsPerChannel.b == 8 && bitsPerChannel.a == 8 && dataType == TextureDataType::UNSIGNED_CHAR)
            {
                return GL_RGBA8;
            }
            else if (bitsPerChannel.r == 10 && bitsPerChannel.g == 10 && bitsPerChannel.b == 10 && bitsPerChannel.a == 2 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_RGB10_A2;
            }
            else if (bitsPerChannel.r == 16 && bitsPerChannel.g == 16 && bitsPerChannel.b == 16 && bitsPerChannel.a == 16 && dataType == TextureDataType::UNSIGNED_SHORT)
            {
                return GL_RGBA16F;
            }
            else if (bitsPerChannel.r == 32 && bitsPerChannel.g == 32 && bitsPerChannel.b == 32 && bitsPerChannel.a == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_RGBA32F;
            }
            else if (bitsPerChannel.r == 32 && bitsPerChannel.g == 32 && bitsPerChannel.b == 32 && bitsPerChannel.a == 32 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_RGBA32UI;
            }
            break;
        case TextureFormat::DEPTH_STENCIL:
            if (bitsPerChannel.r == 24 && bitsPerChannel.g == 8 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_DEPTH24_STENCIL8;
            }
            break;
        case TextureFormat::DEPTH:
            if (bitsPerChannel.r == 16 && dataType == TextureDataType::UNSIGNED_SHORT)
            {
                return GL_DEPTH_COMPONENT16;
            }
            else if (bitsPerChannel.r == 24 && dataType == TextureDataType::UNSIGNED_INT)
            {
                return GL_DEPTH_COMPONENT24;
            }
            else if (bitsPerChannel.r == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_DEPTH_COMPONENT32F;
            }
            break;
        }
        FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled texture format %d", (int)format);
        return 0;
    }

    inline GLenum GetOpenGLFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R:
            return GL_RED;
        case TextureFormat::RG:
            return GL_RG;
        case TextureFormat::RGB:
            return GL_RGB;
        case TextureFormat::RGBA:
            return GL_RGBA;
#if defined GL_BRG
        case TextureFormat::BGR:
            return GL_BGR;
#endif
#if defined GL_BGRA
        case TextureFormat::BGRA:
            return GL_BGRA;
#endif
        case TextureFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case TextureFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled texture format %d", (int)format);
            return 0;
        }
    }

    inline GLenum GetOpenGLDataType(TextureDataType dataType, const BitsPerChannel &rBitsPerChannel)
    {
        switch (dataType)
        {
        case TextureDataType::UNSIGNED_CHAR:
            return GL_UNSIGNED_BYTE;
        case TextureDataType::UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case TextureDataType::UNSIGNED_INT:
            if (rBitsPerChannel.r == 24 && rBitsPerChannel.g == 8)
            {
                return GL_UNSIGNED_INT_24_8;
            }
            else
            {
                return GL_UNSIGNED_INT;
            }
        case TextureDataType::FLOAT:
            return GL_FLOAT;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Unhandled texture data type %d", (int)dataType);
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "OpenGL: Unhandled debug output message source %d", (int)source);
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "OpenGL: Unhandled debug output message type %d", (int)type);
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "OpenGL: Unhandled debug output message severity %d", (int)severity);
            return nullptr;
        }
    }

}

#endif

#endif