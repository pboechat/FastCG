#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLBuffer.h>
#include <FastCG/OpenGLExceptions.h>

namespace
{
    GLenum GetOpenGLTarget(FastCG::BufferType type)
    {
        switch (type)
        {
        case FastCG::BufferType::UNIFORM:
            return GL_UNIFORM_BUFFER;
        case FastCG::BufferType::VERTEX_ATTRIBUTE:
            return GL_ARRAY_BUFFER;
        case FastCG::BufferType::INDICES:
            return GL_ELEMENT_ARRAY_BUFFER;
        default:
            return 0;
        }
    }

    GLenum GetOpenGLUsage(FastCG::BufferUsage usage)
    {
        switch (usage)
        {
        case FastCG::BufferUsage::STATIC:
            return GL_STATIC_DRAW;
        case FastCG::BufferUsage::DYNAMIC:
            return GL_DYNAMIC_DRAW;
        default:
            return 0;
        }
    }
}

namespace FastCG
{
    OpenGLBuffer::OpenGLBuffer(const BufferArgs &rArgs) : BaseBuffer(rArgs)
    {
        auto target = GetOpenGLTarget(mType);

        glGenBuffers(1, &mBufferId);
        glBindBuffer(target, mBufferId);
#ifdef _DEBUG
        {
            auto bufferLabel = mName + " (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
        }
#endif
        if (rArgs.dataSize > 0)
        {
            SetData(rArgs.dataSize, rArgs.pData);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLBuffer::SetData(size_t size, const void *pData) const
    {
        glBufferData(GetOpenGLTarget(mType), (GLsizeiptr)size, pData, GetOpenGLUsage(mUsage));
    }

    void OpenGLBuffer::SetSubData(size_t offset, size_t size, const void *pData) const
    {
        auto target = GetOpenGLTarget(mType);
        glBindBuffer(target, mBufferId);
        glBufferSubData(target, (GLintptr)offset, (GLsizeiptr)size, pData);
    }

    void OpenGLBuffer::Bind() const
    {
        glBindBuffer(GetOpenGLTarget(mType), mBufferId);
    }

    void OpenGLBuffer::BindBase(GLuint location) const
    {
        glBindBufferBase(GetOpenGLTarget(mType), location, mBufferId);
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        if (mBufferId != ~0u)
        {
            glDeleteBuffers(1, &mBufferId);
        }
    }

}

#endif