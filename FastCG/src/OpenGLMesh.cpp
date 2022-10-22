#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    OpenGLMesh::OpenGLMesh(const MeshArgs &rArgs) : BaseMesh(rArgs)
    {
        if (!mArgs.vertices.empty())
        {
            // create vertices buffer and upload data
            glGenBuffers(1, &mVerticesBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mArgs.name + " Vertices (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mVerticesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mArgs.vertices.size() * sizeof(glm::vec3), &mArgs.vertices[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mArgs.normals.empty())
        {
            // create normals buffer and upload data
            glGenBuffers(1, &mNormalsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mArgs.name + " Normals (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mNormalsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mArgs.normals.size() * sizeof(glm::vec3), &mArgs.normals[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mArgs.uvs.empty())
        {
            // create uvs buffer and upload data
            glGenBuffers(1, &mUVsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mUVsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mArgs.name + " UVs (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mUVsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mArgs.uvs.size() * sizeof(glm::vec2), &mArgs.uvs[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mArgs.tangents.empty())
        {
            // create tangents buffer and upload data
            glGenBuffers(1, &mTangentsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mTangentsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mArgs.name + " Tangents (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mTangentsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mArgs.tangents.size() * sizeof(glm::vec4), &mArgs.tangents[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        glGenBuffers(1, &mIndicesBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferId);
#ifdef _DEBUG
        {
            std::string bufferLabel = mArgs.name + " Indices (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mIndicesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
        }
#endif
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mArgs.indices.size() * sizeof(uint32_t), &mArgs.indices[0], GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();

        // create vertex array with all previous buffers attached
        glGenVertexArrays(1, &mVertexArrayId);
        glBindVertexArray(mVertexArrayId);
#ifdef _DEBUG
        {
            std::string vertexArrayLabel = mArgs.name + " (GL_VERTEX_ARRAY)";
            glObjectLabel(GL_VERTEX_ARRAY, mVertexArrayId, (GLsizei)vertexArrayLabel.size(), vertexArrayLabel.c_str());
        }
#endif

        if (!mArgs.vertices.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::VERTICES_ATTRIBUTE_INDEX);
        }
        if (!mArgs.normals.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::NORMALS_ATTRIBUTE_INDEX);
        }
        if (!mArgs.uvs.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::UVS_ATTRIBUTE_INDEX);
        }
        if (!mArgs.tangents.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::TANGENTS_ATTRIBUTE_INDEX);
        }

        if (!mArgs.vertices.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
            glVertexAttribPointer(OpenGLShader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mArgs.normals.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferId);
            glVertexAttribPointer(OpenGLShader::NORMALS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mArgs.uvs.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mUVsBufferId);
            glVertexAttribPointer(OpenGLShader::UVS_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mArgs.tangents.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mTangentsBufferId);
            glVertexAttribPointer(OpenGLShader::TANGENTS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    OpenGLMesh::~OpenGLMesh()
    {
        if (mIndicesBufferId != ~0u)
        {
            glDeleteBuffers(1, &mIndicesBufferId);
        }
        if (mTangentsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mTangentsBufferId);
        }
        if (mUVsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mUVsBufferId);
        }
        if (mNormalsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mNormalsBufferId);
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

    void OpenGLMesh::Draw() const
    {
        glBindVertexArray(mVertexArrayId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferId);
        glDrawElements(GL_TRIANGLES, (GLsizei)mArgs.indices.size(), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

#endif