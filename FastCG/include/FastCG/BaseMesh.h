#ifndef FASTCG_BASE_MESH_H
#define FASTCG_BASE_MESH_H

#include <FastCG/AABB.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace FastCG
{
	class BaseMesh
	{
	public:
		BaseMesh() = default;
		virtual ~BaseMesh() = default;

		void Initialize(const std::string &rName, const std::vector<uint32_t> &indices);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint32_t> &indices);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec4> &tangents, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices);
		void CalculateNormals();
		void CalculateTangents();

		inline size_t GetNumberOfTriangles() const
		{
			return mIndices.size() / 3;
		}

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline const std::vector<glm::vec3> &GetVertices() const
		{
			return mVertices;
		}

		inline const std::vector<glm::vec3> &GetNormals() const
		{
			return mNormals;
		}

		inline const std::vector<glm::vec2> &GetUVs() const
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

		inline const std::vector<uint32_t> &GetIndices() const
		{
			return mIndices;
		}

		inline const AABB &GetBounds() const
		{
			return mBounds;
		}

		virtual void Draw() = 0;

	protected:
		std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec3> mNormals;
		std::vector<glm::vec2> mUVs;
		std::vector<glm::vec4> mTangents;
		std::vector<uint32_t> mIndices;
		AABB mBounds;

		void CalculateBounds();
	};

}

#endif
