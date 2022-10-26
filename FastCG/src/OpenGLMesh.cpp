#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    OpenGLMesh::OpenGLMesh(const MeshArgs &rArgs) : BaseMesh(rArgs)
    {
        if (!mVertices.empty())
        {
            mpVerticesBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Vertices",
                                                                                  BufferType::VERTEX_ATTRIBUTE,
                                                                                  BufferUsage::STATIC,
                                                                                  mVertices.size() * sizeof(glm::vec3),
                                                                                  &mVertices[0]});
        }

        if (!mNormals.empty())
        {
            mpNormalsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Normals",
                                                                                 BufferType::VERTEX_ATTRIBUTE,
                                                                                 BufferUsage::STATIC,
                                                                                 mNormals.size() * sizeof(glm::vec3), &mNormals[0]});
        }

        if (!mUVs.empty())
        {
            mpUVsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " UVs",
                                                                             BufferType::VERTEX_ATTRIBUTE,
                                                                             BufferUsage::STATIC,
                                                                             mUVs.size() * sizeof(glm::vec2), &mUVs[0]});
        }

        if (!mTangents.empty())
        {
            mpTangentsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Tangents",
                                                                                  BufferType::VERTEX_ATTRIBUTE,
                                                                                  BufferUsage::STATIC,
                                                                                  mTangents.size() * sizeof(glm::vec4), &mTangents[0]});
        }

        if (!mColors.empty())
        {
            mpColorsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Colors",
                                                                                BufferType::VERTEX_ATTRIBUTE,
                                                                                BufferUsage::STATIC,
                                                                                mColors.size() * sizeof(glm::vec4), &mColors[0]});
        }

        mpIndicesBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Indices",
                                                                             BufferType::INDICES,
                                                                             BufferUsage::STATIC,
                                                                             mIndices.size() * sizeof(uint32_t), &mIndices[0]});

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
            mpVerticesBuffer->Bind();
            glVertexAttribPointer(OpenGLShader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mNormals.empty())
        {
            mpNormalsBuffer->Bind();
            glVertexAttribPointer(OpenGLShader::NORMALS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_TRUE, 0, 0);
        }
        if (!mUVs.empty())
        {
            mpUVsBuffer->Bind();
            glVertexAttribPointer(OpenGLShader::UVS_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mTangents.empty())
        {
            mpTangentsBuffer->Bind();
            glVertexAttribPointer(OpenGLShader::TANGENTS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
        }
        if (!mColors.empty())
        {
            mpColorsBuffer->Bind();
            glVertexAttribPointer(OpenGLShader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    OpenGLMesh::~OpenGLMesh()
    {
        if (mpIndicesBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpIndicesBuffer);
        }
        if (mpColorsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpColorsBuffer);
        }
        if (mpTangentsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpTangentsBuffer);
        }
        if (mpUVsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpUVsBuffer);
        }
        if (mpNormalsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpNormalsBuffer);
        }
        if (mpVerticesBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpVerticesBuffer);
        }
        if (mVertexArrayId != ~0u)
        {
            glDeleteBuffers(1, &mVertexArrayId);
        }
    }

    void OpenGLMesh::SetVertices(const std::vector<glm::vec3> &vertices)
    {
        mVertices = vertices;
        mpVerticesBuffer->SetData(mVertices.size() * sizeof(glm::vec3), &mVertices[0]);
    }

    void OpenGLMesh::SetNormals(const std::vector<glm::vec3> &normals)
    {
        mNormals = normals;
        mpNormalsBuffer->SetData(mVertices.size() * sizeof(glm::vec3), &mVertices[0]);
    }

    void OpenGLMesh::SetUVs(const std::vector<glm::vec2> &uvs)
    {
        mUVs = uvs;
        mpUVsBuffer->SetData(mUVs.size() * sizeof(glm::vec2), &mUVs[0]);
    }

    void OpenGLMesh::SetTangents(const std::vector<glm::vec4> &tangents)
    {
        mTangents = tangents;
        mpTangentsBuffer->SetData(mTangents.size() * sizeof(glm::vec4), &mTangents[0]);
    }

    void OpenGLMesh::SetColors(const std::vector<glm::vec4> &colors)
    {
        mColors = colors;
        mpColorsBuffer->SetData(mColors.size() * sizeof(glm::vec4), &mColors[0]);
    }

    void OpenGLMesh::SetIndices(const std::vector<uint32_t> &indices)
    {
        mIndices = indices;
        mpIndicesBuffer->SetData(mIndices.size() * sizeof(uint32_t), &mIndices[0]);
    }

    void OpenGLMesh::Draw() const
    {
        glBindVertexArray(mVertexArrayId);
        mpIndicesBuffer->Bind();
        glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, 0);
    }
}

#endif