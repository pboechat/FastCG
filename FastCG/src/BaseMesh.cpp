#include <FastCG/BaseMesh.h>
#include <FastCG/MathT.h>

namespace FastCG
{
	void BaseMesh::Initialize(const std::string &rName, const std::vector<uint32_t> &indices)
	{
		mName = rName;
		mIndices = indices;
	}

	void BaseMesh::Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices)
	{
		mName = rName;
		mVertices = vertices;
		mIndices = indices;
		CalculateBounds();
	}

	void BaseMesh::Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<uint32_t> &indices)
	{
		assert(vertices.size() == normals.size());
		mName = rName;
		mVertices = vertices;
		mNormals = normals;
		mIndices = indices;
		CalculateBounds();
	}

	void BaseMesh::Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices)
	{
		assert(vertices.size() == uvs.size());
		mName = rName;
		CalculateBounds();
	}

	void BaseMesh::Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices)
	{
		assert(vertices.size() == normals.size() && vertices.size() == uvs.size());
		mName = rName;
		mVertices = vertices;
		mNormals = normals;
		mUVs = uvs;
		mIndices = indices;
		CalculateBounds();
	}

	void BaseMesh::Initialize(const std::string &rName, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec4> &tangents, const std::vector<glm::vec2> &uvs, const std::vector<uint32_t> &indices)
	{
		assert(vertices.size() == normals.size() && vertices.size() == uvs.size() && vertices.size() == tangents.size());
		mName = rName;
		mVertices = vertices;
		mNormals = normals;
		mTangents = tangents;
		mIndices = indices;
		mUVs = uvs;
		CalculateBounds();
	}

	void BaseMesh::CalculateNormals()
	{
		mNormals.resize(mVertices.size());
		for (size_t i = 0; i < mIndices.size(); i += 3)
		{
			uint32_t i1 = mIndices[i];
			uint32_t i2 = mIndices[i + 1];
			uint32_t i3 = mIndices[i + 2];

			const auto &v1 = mVertices[i1];
			const auto &v2 = mVertices[i2];
			const auto &v3 = mVertices[i3];

			auto n = glm::cross(v2 - v1, v3 - v1);
			mNormals[i1] += n;
			mNormals[i2] += n;
			mNormals[i3] += n;
		}

		for (auto &rNormal : mNormals)
		{
			rNormal = glm::normalize(rNormal);
		}
	}

	void BaseMesh::CalculateTangents()
	{
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

			auto &v1 = mVertices[i1];
			auto &v2 = mVertices[i2];
			auto &v3 = mVertices[i3];

			auto &w1 = mUVs[i1];
			auto &w2 = mUVs[i2];
			auto &w3 = mUVs[i3];

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
			auto &normal = mNormals[i];
			auto &tangent = tangents1[i];

			// gram-schmidt orthogonalization
			mTangents[i] = glm::vec4(glm::normalize(tangent - normal * glm::dot(normal, tangent)), 0.0f);

			// calculate handedness
			mTangents[i].w = (glm::dot(glm::cross(normal, tangent), tangents2[i]) < 0.0f) ? -1.0f : 1.0f;
		}
	}

	void BaseMesh::CalculateBounds()
	{
		if (mVertices.empty())
		{
			return;
		}

		mBounds.min.x = mBounds.min.y = mBounds.min.z = 1e10f;
		mBounds.max.x = mBounds.max.y = mBounds.max.z = -1e10f;

		for (const auto &rVertex : mVertices)
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