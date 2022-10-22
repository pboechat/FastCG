#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    OpenGLMesh::OpenGLMesh(const MeshArgs &rArgs) : BaseMesh(rArgs)
    {
        if (!mVertices.empty())
        {
            // create vertices buffer and upload data
            glGenBuffers(1, &mVerticesBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mName + " Vertices (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mVerticesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mNormals.empty())
        {
            // create normals buffer and upload data
            glGenBuffers(1, &mNormalsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mName + " Normals (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mNormalsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3), &mNormals[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mUVs.empty())
        {
            // create uvs buffer and upload data
            glGenBuffers(1, &mUVsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mUVsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mName + " UVs (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mUVsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mUVs.size() * sizeof(glm::vec2), &mUVs[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mTangents.empty())
        {
            // create tangents buffer and upload data
            glGenBuffers(1, &mTangentsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mTangentsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mName + " Tangents (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mTangentsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mTangents.size() * sizeof(glm::vec4), &mTangents[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        if (!mColors.empty())
        {
            // create colors buffer and upload data
            glGenBuffers(1, &mColorsBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
#ifdef _DEBUG
            {
                std::string bufferLabel = mName + " Colors (GL_BUFFER)";
                glObjectLabel(GL_BUFFER, mColorsBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
            }
#endif
            glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(glm::vec4), &mColors[0], GL_STATIC_DRAW);

            FASTCG_CHECK_OPENGL_ERROR();
        }

        glGenBuffers(1, &mIndicesBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferId);
#ifdef _DEBUG
        {
            std::string bufferLabel = mName + " Indices (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mIndicesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
        }
#endif
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), &mIndices[0], GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();

        // create vertex array with all previous buffers attached
        glGenVertexArrays(1, &mVertexArrayId);
        glBindVertexArray(mVertexArrayId);
#ifdef _DEBUG
        {
            std::string vertexArrayLabel = mName + " (GL_VERTEX_ARRAY)";
            glObjectLabel(GL_VERTEX_ARRAY, mVertexArrayId, (GLsizei)vertexArrayLabel.size(), vertexArrayLabel.c_str());
        }
#endif

        if (!mVertices.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::VERTICES_ATTRIBUTE_INDEX);
        }
        if (!mNormals.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::NORMALS_ATTRIBUTE_INDEX);
        }
        if (!mUVs.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::UVS_ATTRIBUTE_INDEX);
        }
        if (!mTangents.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::TANGENTS_ATTRIBUTE_INDEX);
        }
        if (!mColors.empty())
        {
            glEnableVertexAttribArray(OpenGLShader::COLORS_ATTRIBUTE_INDEX);
        }

        if (!mVertices.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
            glVertexAttribPointer(OpenGLShader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mNormals.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferId);
            glVertexAttribPointer(OpenGLShader::NORMALS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_TRUE, 0, 0);
        }
        if (!mUVs.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mUVsBufferId);
            glVertexAttribPointer(OpenGLShader::UVS_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mTangents.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mTangentsBufferId);
            glVertexAttribPointer(OpenGLShader::TANGENTS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mColors.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
            glVertexAttribPointer(OpenGLShader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    OpenGLMesh::~OpenGLMesh()
    {
        if (mIndicesBufferId != ~0u)
        {
            glDeleteBuffers(1, &mIndicesBufferId);
        }
        if (mColorsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mColorsBufferId);
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

    void OpenGLMesh::SetVertices(const std::vector<glm::vec3> &vertices)
    {
        mVertices = vertices;
        glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);
    }

    void OpenGLMesh::SetNormals(const std::vector<glm::vec3> &normals)
    {
        mNormals = normals;
        glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferId);
        glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3), &mNormals[0], GL_STATIC_DRAW);
    }

    void OpenGLMesh::SetUVs(const std::vector<glm::vec2> &uvs)
    {
        mUVs = uvs;
        glBindBuffer(GL_ARRAY_BUFFER, mUVsBufferId);
        glBufferData(GL_ARRAY_BUFFER, mUVs.size() * sizeof(glm::vec2), &mUVs[0], GL_STATIC_DRAW);
    }

    void OpenGLMesh::SetTangents(const std::vector<glm::vec4> &tangents)
    {
        mTangents = tangents;
        glBindBuffer(GL_ARRAY_BUFFER, mTangentsBufferId);
        glBufferData(GL_ARRAY_BUFFER, mTangents.size() * sizeof(glm::vec4), &mTangents[0], GL_STATIC_DRAW);
    }

    void OpenGLMesh::SetColors(const std::vector<glm::vec4> &colors)
    {
        mColors = colors;
        glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
        glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(glm::vec4), &mColors[0], GL_STATIC_DRAW);
    }

    void OpenGLMesh::SetIndices(const std::vector<uint32_t> &indices)
    {
        mIndices = indices;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t), &mIndices[0], GL_STATIC_DRAW);
    }

    void OpenGLMesh::Draw() const
    {
        glBindVertexArray(mVertexArrayId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferId);
        glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

#endif