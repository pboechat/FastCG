#include <FastCG/Shader.h>
#include <FastCG/Mesh.h>
#include <FastCG/MathT.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

namespace FastCG
{
	Mesh::Mesh(const std::string& rName, const std::vector<uint32_t>& indexes) :
		mName(rName),
		mIndices(indexes)
	{
	}

	Mesh::Mesh(const std::string& rName, const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indexes) :
		mName(rName),
		mVertices(vertices),
		mIndices(indexes)
	{
		CalculateBounds();
	}

	Mesh::Mesh(const std::string& rName, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<uint32_t>& indexes) :
		mName(rName),
		mVertices(vertices),
		mNormals(normals),
		mIndices(indexes)
	{
		assert(vertices.size() == normals.size());
		CalculateBounds();
	}

	Mesh::Mesh(const std::string& rName, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<uint32_t>& indexes) :
		mName(rName)
	{
		assert(vertices.size() == uvs.size());
		CalculateBounds();
	}

	Mesh::Mesh(const std::string& rName, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const std::vector<uint32_t>& indices) :
		mName(rName),
		mVertices(vertices),
		mNormals(normals),
		mUVs(uvs),
		mIndices(indices)
	{
		assert(vertices.size() == normals.size() && vertices.size() == uvs.size());
		CalculateBounds();
	}

	Mesh::Mesh(const std::string& rName, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& tangents, const std::vector<glm::vec2>& uvs, const std::vector<uint32_t>& indices) :
		mName(rName),
		mVertices(vertices),
		mNormals(normals),
		mTangents(tangents),
		mIndices(indices),
		mUVs(uvs)
	{
		assert(vertices.size() == normals.size() && vertices.size() == uvs.size() && vertices.size() == tangents.size());
		CalculateBounds();
	}

	Mesh::~Mesh()
	{
		DeallocateResources();
	}

	void Mesh::AllocateResources()
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
			glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
		}
		if (!mNormals.empty())
		{
			glEnableVertexAttribArray(Shader::NORMALS_ATTRIBUTE_INDEX);
		}
		if (!mUVs.empty())
		{
			glEnableVertexAttribArray(Shader::UVS_ATTRIBUTE_INDEX);
		}
		if (!mTangents.empty())
		{
			glEnableVertexAttribArray(Shader::TANGENTS_ATTRIBUTE_INDEX);
		}

		if (!mVertices.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
			glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if (!mNormals.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferId);
			glVertexAttribPointer(Shader::NORMALS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if (!mUVs.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, mUVsBufferId);
			glVertexAttribPointer(Shader::UVS_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if (!mTangents.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, mTangentsBufferId);
			glVertexAttribPointer(Shader::TANGENTS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
		}

		FASTCG_CHECK_OPENGL_ERROR();
	}

	void Mesh::DeallocateResources()
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

	void Mesh::Draw()
	{
		if (mVertexArrayId == ~0u)
		{
			AllocateResources();
		}

		glBindVertexArray(mVertexArrayId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferId);
		glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::CalculateNormals()
	{
		if (mNormalsBufferId != ~0u)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Cannot calculate normals after normals buffer is already created (Mesh: %s)", mName.c_str());
		}

		mNormals.resize(mVertices.size());
		for (size_t i = 0; i < mIndices.size(); i += 3)
		{
			uint32_t i1 = mIndices[i];
			uint32_t i2 = mIndices[i + 1];
			uint32_t i3 = mIndices[i + 2];

			const auto& v1 = mVertices[i1];
			const auto& v2 = mVertices[i2];
			const auto& v3 = mVertices[i3];

			auto n = glm::cross(v2 - v1, v3 - v1);
			mNormals[i1] += n;
			mNormals[i2] += n;
			mNormals[i3] += n;
		}

		for (auto& rNormal : mNormals)
		{
			rNormal = glm::normalize(rNormal);
		}
	}

	void Mesh::CalculateTangents()
	{
		if (mVertexArrayId != ~0u)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Cannot calculate tangents after VAO is already created: %d", mVertexArrayId);
		}

		// if there's no vertices, exit
		if (mVertices.empty())
		{
			return;
		}

		// if there's no UV, exit
		if (mUVs.empty())
		{
			return;
		}

		mTangents.resize(mVertices.size());
		std::vector<glm::vec3> tangents1(mVertices.size());
		std::vector<glm::vec3> tangents2(mVertices.size());

		for (size_t i = 0; i < mIndices.size(); i += 3)
		{
			auto i1 = mIndices[i];
			auto i2 = mIndices[i + 1];
			auto i3 = mIndices[i + 2];

			auto& v1 = mVertices[i1];
			auto& v2 = mVertices[i2];
			auto& v3 = mVertices[i3];

			auto& w1 = mUVs[i1];
			auto& w2 = mUVs[i2];
			auto& w3 = mUVs[i3];

			auto x1 = v2.x - v1.x;
			auto x2 = v3.x - v1.x;

			auto y1 = v2.y - v1.y;
			auto y2 = v3.y - v1.y;

			auto z1 = v2.z - v1.z;
			auto z2 = v3.z - v1.z;

			auto s1 = w2.x - w1.x;
			auto s2 = w3.x - w1.x;

			auto t1 = w2.y - w1.y;
			auto t2 = w3.y - w1.y;

			auto r = 1.0f / (s1 * t2 - s2 * t1);

			glm::vec3 sdir((t2 * x1 - t1 * x2) * r,
				(t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);

			glm::vec3 tdir((s1 * x2 - s2 * x1) * r,
				(s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tangents1[i1] += sdir;
			tangents1[i2] += sdir;
			tangents1[i3] += sdir;

			tangents2[i1] += tdir;
			tangents2[i2] += tdir;
			tangents2[i3] += tdir;
		}

		for (size_t i = 0; i < mVertices.size(); i++)
		{
			auto& normal = mNormals[i];
			auto& tangent = tangents1[i];

			// gram-schmidt orthogonalization
			mTangents[i] = glm::vec4(glm::normalize(tangent - normal * glm::dot(normal, tangent)), 0.0f);

			// calculate handedness
			mTangents[i].w = (glm::dot(glm::cross(normal, tangent), tangents2[i]) < 0.0f) ? -1.0f : 1.0f;
		}
	}

	void Mesh::CalculateBounds()
	{
		if (mVertices.empty())
		{
			return;
		}

		mBounds.min.x = mBounds.min.y = mBounds.min.z = 1e10f;
		mBounds.max.x = mBounds.max.y = mBounds.max.z = -1e10f;

		for (const auto& rVertex : mVertices)
		{
			mBounds.min.x = MathF::Min(mBounds.min.x, rVertex.x);
			mBounds.min.y = MathF::Min(mBounds.min.y, rVertex.y);
			mBounds.min.z = MathF::Min(mBounds.min.z, rVertex.z);
			mBounds.max.x = MathF::Max(mBounds.max.x, rVertex.x);
			mBounds.max.y = MathF::Max(mBounds.max.y, rVertex.y);
			mBounds.max.z = MathF::Max(mBounds.max.z, rVertex.z);
		}
	}

}