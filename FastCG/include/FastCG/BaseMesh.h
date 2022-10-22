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
			return mArgs.indices.size() / 3;
		}

		inline const std::string &GetName() const
		{
			return mArgs.name;
		}

		inline const std::vector<glm::vec3> &GetVertices() const
		{
			return mArgs.vertices;
		}

		inline const std::vector<glm::vec3> &GetNormals() const
		{
			return mArgs.normals;
		}

		inline const std::vector<glm::vec2> &GetUVs() const
		{
			return mArgs.uvs;
		}

		inline const std::vector<glm::vec4> &GetTangents() const
		{
			return mArgs.tangents;
		}

		inline const std::vector<uint32_t> &GetIndices() const
		{
			return mArgs.indices;
		}

		inline const AABB &GetBounds() const
		{
			return mBounds;
		}

	protected:
		const MeshArgs mArgs;
		AABB mBounds;

		BaseMesh(const MeshArgs &rArgs) : mArgs{rArgs.name,
												rArgs.vertices,
												rArgs.calculateNormals ? CalculateNormals(rArgs.vertices, rArgs.indices) : rArgs.normals,
												rArgs.uvs,
												rArgs.calculateTangents ? CalculateTangents(rArgs.vertices, rArgs.normals, rArgs.uvs, rArgs.indices) : rArgs.tangents,
												rArgs.indices,
												rArgs.calculateBounds,
												rArgs.calculateNormals,
												rArgs.calculateTangents}
		{
			if (mArgs.calculateBounds)
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
