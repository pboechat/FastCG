#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>

#include <cstring>

namespace FastCG
{
    OpenGLBuffer::OpenGLBuffer(const Args &rArgs) : BaseBuffer(rArgs)
    {
        auto target = GetOpenGLTarget(mUsage);

        glGenBuffers(1, &mBufferId);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't generate buffer (buffer: %s)", rArgs.name.c_str());

        glBindBuffer(target, mBufferId);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't bind buffer (buffer: %s)", rArgs.name.c_str());

#ifdef _DEBUG
        {
            auto bufferLabel = mName + " (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
        }
#endif
        if (rArgs.dataSize > 0)
        {
            glBufferData(target, (GLsizeiptr)rArgs.dataSize, rArgs.pData, GetOpenGLUsageHint(mUsage));
            FASTCG_CHECK_OPENGL_ERROR("Couldn't create buffer data store (buffer: %s, usage: %d)", rArgs.name.c_str(), (int)mUsage);

            if (rArgs.pData != nullptr)
            {
                glBufferSubData(target, 0, (GLsizeiptr)rArgs.dataSize, (const GLvoid *)rArgs.pData);
                FASTCG_CHECK_OPENGL_ERROR("Couldn't update the buffer data store (buffer: %s)", rArgs.name.c_str());
            }
        }
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
