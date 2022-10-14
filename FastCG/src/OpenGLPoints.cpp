#ifdef FASTCG_OPENGL

#include <FastCG/Shader.h>
#include <FastCG/OpenGLPoints.h>
#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    OpenGLPoints::~OpenGLPoints()
    {
        DeallocateResources();
    }

    void OpenGLPoints::AllocateResources()
    {
        // create vertex buffer and upload data
        glGenBuffers(1, &mVerticesBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
#ifdef _DEBUG
        {
            std::string bufferLabel = GetName() + " Vertices (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mVerticesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
        }
#endif
        glBufferData(GL_ARRAY_BUFFER, GetVertices().size() * sizeof(glm::vec3), &GetVertices()[0], GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();

        // create colors buffer and upload data
        glGenBuffers(1, &mColorsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
#ifdef _DEBUG
        {
            std::string bufferLabel = GetName() + " Colors (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mVerticesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
        }
#endif
        glBufferData(GL_ARRAY_BUFFER, GetColors().size() * sizeof(glm::vec4), &GetColors()[0], GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();

        // create vertex array with all previous buffers attached
        glGenVertexArrays(1, &mVertexArrayId);
        glBindVertexArray(mVertexArrayId);
#ifdef _DEBUG
        {
            std::string vertexArrayLabel = GetName() + " (GL_VERTEX_ARRAY)";
            glObjectLabel(GL_VERTEX_ARRAY, mVertexArrayId, (GLsizei)vertexArrayLabel.size(), vertexArrayLabel.c_str());
        }
#endif

        glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
        glEnableVertexAttribArray(Shader::COLORS_ATTRIBUTE_INDEX);

        glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
        glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
        glVertexAttribPointer(Shader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);

        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLPoints::DeallocateResources()
    {
        if (mColorsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mColorsBufferId);
        }
        if (mVerticesBufferId != ~0u)
        {
            glDeleteBuffers(1, &mVerticesBufferId);
        }
        if (mVertexArrayId != ~0u)
        {
            glDeleteBuffers(1, &mVertexArrayId);
        }
    }

    void OpenGLPoints::Draw()
    {
        if (mVertexArrayId == ~0u)
        {
            AllocateResources();
        }

        glBindVertexArray(mVertexArrayId);
        glDrawArrays(GL_POINTS, 0, (GLsizei)GetVertices().size());
        glBindVertexArray(0);
    }
}

#endif