#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>

#include <cstring>

namespace FastCG
{
    OpenGLBuffer::OpenGLBuffer(const BufferArgs &rArgs) : BaseBuffer(rArgs)
    {
        auto target = GetOpenGLTarget(mUsage);

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
            glBufferData(target, (GLsizeiptr)rArgs.dataSize, rArgs.pData, GetOpenGLUsageHint(mUsage));
            if (rArgs.pData != nullptr)
            {
                glBufferSubData(target, 0, (GLsizeiptr)rArgs.dataSize, (const GLvoid *)rArgs.pData);
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

