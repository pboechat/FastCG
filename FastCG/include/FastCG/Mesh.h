#ifndef FASTCG_MESH_H_
#define FASTCG_MESH_H_

#include <FastCG/AABB.h>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>
#include <cstdint>

namespace FastCG
{
	class Mesh
	{
	public:
		Mesh(const std::string &rName, const std::vector<uint32_t> &indexes);
		Mesh(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indexes);
		Mesh(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint32_t> &indexes);
		Mesh(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indexes);
		Mesh(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indexes);
		Mesh(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec4> &tangents, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indexes);
		virtual ~Mesh();

		void CalculateNormals();
		void CalculateTangents();

		inline size_t GetNumberOfTriangles() const
		{
			return mIndices.size() / 3;
		}

		inline const std::vector<glm::vec3> GetVertices() const
		{
			return mVertices;
		}

		inline const std::vector<glm::vec3> GetNormals() const
		{
			return mNormals;
		}

		inline const std::vector<glm::vec2> GetUVs() const
		{
			return mUVs;
		}

		inline const std::vector<glm::vec4> &GetTangents() const
		{
			return mTangents;
		}

		inline void SetTangents(const std::vector<glm::vec4> &rTangents)
		{
			mTangents = rTangents;
		}

		inline const std::vector<uint32_t> GetIndices() const
		{
			return mIndices;
		}

		inline const AABB &GetBounds() const
		{
			return mBounds;
		}

		void Draw();
		void AllocateResources();
		void DeallocateResources();

	private:
		std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec3> mNormals;
		std::vector<glm::vec2> mUVs;
		std::vector<glm::vec4> mTangents;
		std::vector<uint32_t> mIndices;
		GLuint mVertexArrayId{~0u};
		GLuint mVerticesBufferId{~0u};
		GLuint mNormalsBufferId{~0u};
		GLuint mUVsBufferId{~0u};
		GLuint mTangentsBufferId{~0u};
		GLuint mIndicesBufferId{~0u};
		AABB mBounds;

		void CalculateBounds();
	};

}

#endif
