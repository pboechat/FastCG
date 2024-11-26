#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/OpenGL/OpenGLErrorHandling.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>

#include <cstring>

namespace FastCG
{
    OpenGLBuffer::OpenGLBuffer(const Args &rArgs) : BaseBuffer(rArgs)
    {
        auto target = GetOpenGLTarget(mUsage);

        FASTCG_CHECK_OPENGL_CALL(glGenBuffers(1, &mBufferId));

        FASTCG_CHECK_OPENGL_CALL(glBindBuffer(target, mBufferId));

#if _DEBUG
        {
            auto bufferLabel = mName + " (GL_BUFFER)";
            FASTCG_CHECK_OPENGL_CALL(
                glObjectLabel(GL_BUFFER, mBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str()));
        }
#endif
        if (rArgs.dataSize > 0)
        {
            FASTCG_CHECK_OPENGL_CALL(
                glBufferData(target, (GLsizeiptr)rArgs.dataSize, rArgs.pData, GetOpenGLUsageHint(mUsage)));

            if (rArgs.pData != nullptr)
            {
                FASTCG_CHECK_OPENGL_CALL(
                    glBufferSubData(target, 0, (GLsizeiptr)rArgs.dataSize, (const GLvoid *)rArgs.pData));
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
