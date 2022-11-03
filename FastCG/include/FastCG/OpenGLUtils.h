#ifndef FASTCG_OPENGL_UTILS_H
#define FASTCG_OPENGL_UTILS_H

#ifdef FASTCG_OPENGL

#include <FastCG/Exception.h>
#include <FastCG/BaseRenderingContext.h>
#include <FastCG/BaseShader.h>
#include <FastCG/BaseTexture.h>
#include <FastCG/BaseBuffer.h>

#include <GL/glew.h>
#include <GL/gl.h>

#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str) \
    case str:                   \
        return #str

namespace FastCG
{
    inline GLenum GetOpenGLTarget(BufferType type)
    {
        switch (type)
        {
        case BufferType::UNIFORM:
            return GL_UNIFORM_BUFFER;
        case BufferType::VERTEX_ATTRIBUTE:
            return GL_ARRAY_BUFFER;
        case BufferType::INDICES:
            return GL_ELEMENT_ARRAY_BUFFER;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled buffer type: %d", (int)type);
            return (GLenum)0;
        }
    }

    inline GLenum GetOpenGLUsage(BufferUsage usage)
    {
        switch (usage)
        {
        case BufferUsage::STATIC:
            return GL_STATIC_DRAW;
        case BufferUsage::DYNAMIC:
            return GL_DYNAMIC_DRAW;
        case BufferUsage::STREAM:
            return GL_STREAM_DRAW;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled buffer usage: %d", (int)usage);
            return (GLenum)0;
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled vertex data type: %d", (int)type);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled stencil func: %d", (int)stencilFunc);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled face: %d", (int)face);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled compare op: %d", (int)compareOp);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled blend func: %d", (int)blendFunc);
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
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled blend factor: %d", (int)blendFactor);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled primitive type: %d", (int)primitiveType);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled OpenGL shader type");
            return nullptr;
        }
    }

    inline GLenum GetOpenGLShaderType(ShaderType shaderType)
    {
        switch (shaderType)
        {
        case ShaderType::ST_VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::ST_FRAGMENT:
            return GL_FRAGMENT_SHADER;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled shader type");
            return 0;
        }
    }

    inline GLenum GetOpenGLTarget(TextureType type)
    {
        switch (type)
        {
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled texture type: %d", (int)type);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled filter: %d", (int)filter);
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
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled wrapping mode: %d", (int)wrapMode);
            return 0;
        }
    }

    inline GLint GetOpenGLInternalFormat(TextureFormat format, const BitsPerPixel &bitsPerPixel, TextureDataType dataType)
    {
        switch (format)
        {
        case TextureFormat::R:
            if (bitsPerPixel.r == 8)
            {
                return GL_R8;
            }
            else if (bitsPerPixel.r == 16)
            {
                if (dataType == TextureDataType::FLOAT)
                {
                    return GL_R16F;
                }
                else
                {
                    return GL_R16;
                }
            }
            else if (bitsPerPixel.r == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_R32F;
            }
            else
            {
                return GL_RED;
            }
        case TextureFormat::RG:
            if (bitsPerPixel.r == 8 && bitsPerPixel.g == 8)
            {
                return GL_RG8;
            }
            else if (bitsPerPixel.r == 16 && bitsPerPixel.g == 16)
            {
                return GL_RG16;
            }
            else
            {
                return GL_RG;
            }
        case TextureFormat::RGB:
        case TextureFormat::BGR:
            if (bitsPerPixel.r == 8 && bitsPerPixel.g == 8 && bitsPerPixel.b == 8)
            {
                return GL_RGB8;
            }
            else if (bitsPerPixel.r == 10 && bitsPerPixel.g == 10 && bitsPerPixel.b == 10)
            {
                return GL_RGB10;
            }
            else if (bitsPerPixel.r == 12 && bitsPerPixel.g == 12 && bitsPerPixel.b == 12)
            {
                return GL_RGB12;
            }
            else if (bitsPerPixel.r == 16 && bitsPerPixel.g == 16 && bitsPerPixel.b == 16)
            {
                return GL_RGB16;
            }
            else
            {
                return GL_RG;
            }
        case TextureFormat::RGBA:
        case TextureFormat::BGRA:
            if (bitsPerPixel.r == 32 && bitsPerPixel.g == 32 && bitsPerPixel.b == 32 && bitsPerPixel.a == 32 && dataType == TextureDataType::FLOAT)
            {
                return GL_RGBA32F;
            }
            else
            {
                return GL_RGBA;
            }
        case TextureFormat::DEPTH_STENCIL:
            if (bitsPerPixel.r == 24 && bitsPerPixel.g == 8)
            {
                return GL_DEPTH24_STENCIL8;
            }
            else
            {
                return GL_DEPTH_STENCIL;
            }
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled texture format: %d", (int)format);
            return 0;
        }
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
        case TextureFormat::BGR:
            return GL_BGR;
        case TextureFormat::BGRA:
            return GL_BGRA;
        case TextureFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled texture format: %d", (int)format);
            return 0;
        }
    }

    inline GLenum GetOpenGLDataType(TextureDataType dataType, const BitsPerPixel &bitsPerPixel)
    {
        switch (dataType)
        {
        case TextureDataType::FLOAT:
            return GL_FLOAT;
        case TextureDataType::UNSIGNED_CHAR:
            return GL_UNSIGNED_BYTE;
        case TextureDataType::UNSIGNED_INT:
            if (bitsPerPixel.r == 24 && bitsPerPixel.g == 8)
            {
                return GL_UNSIGNED_INT_24_8;
            }
            else
            {
                return GL_UNSIGNED_INT;
            }
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled data type: %d", (int)dataType);
            return 0;
        }
    }

    inline bool IsDepthFormat(TextureFormat format)
    {
        return format == TextureFormat::DEPTH_STENCIL;
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpengGL debug output message source");
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpengGL debug output message type");
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpengGL debug output message severity");
            return nullptr;
        }
    }

}

#endif

#endif