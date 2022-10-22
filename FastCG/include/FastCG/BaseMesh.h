#ifndef FASTCG_BASE_MESH_H
#define FASTCG_BASE_MESH_H

#include <FastCG/AABB.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace FastCG
{
	struct MeshArgs
	{
		std::string name;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec4> colors;
		std::vector<uint32_t> indices;
		bool calculateBounds{true};
		bool calculateNormals{false};
		bool calculateTangents{false};
	};

	class BaseMesh
	{
	public:
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

		inline const std::vector<glm::vec4> &GetColors() const
		{
			return mColors;
		}

		inline const std::vector<uint32_t> &GetIndices() const
		{
			return mIndices;
		}

		inline const AABB &GetBounds() const
		{
			return mBounds;
		}

	protected:
		const std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec3> mNormals;
		std::vector<glm::vec2> mUVs;
		std::vector<glm::vec4> mTangents;
		std::vector<glm::vec4> mColors;
		std::vector<uint32_t> mIndices;
		AABB mBounds;

		BaseMesh(const MeshArgs &rArgs) : mName(rArgs.name),
										  mVertices(rArgs.vertices),
										  mNormals(rArgs.calculateNormals ? CalculateNormals(rArgs.vertices, rArgs.indices) : rArgs.normals),
										  mUVs(rArgs.uvs),
										  mTangents(rArgs.calculateTangents ? CalculateTangents(rArgs.vertices, rArgs.normals, rArgs.uvs, rArgs.indices) : rArgs.tangents),
										  mColors(rArgs.colors),
										  mIndices(rArgs.indices)
		{
			if (rArgs.calculateBounds)
			{
				CalculateBounds();
			}
		}
		virtual ~BaseMesh() = default;

		inline static std::vector<glm::vec3> CalculateNormals(const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices);
		inline static std::vector<glm::vec4> CalculateTangents(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices);
		inline void CalculateBounds();
	};

}

#include <FastCG/BaseMesh.inc>

#endif
