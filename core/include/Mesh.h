#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
	Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs);
	Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& tangents, const std::vector<glm::vec2>& uvs);
	virtual ~Mesh();

	inline bool IsUsingTangents() const
	{
		return mUseTangents;
	}

	inline void SetUseTangents(bool useTangents)
	{
		mUseTangents = useTangents;
	}

	inline const std::vector<glm::vec4>& GetTangents() const
	{
		return mTangents;
	}

	inline void SetTangents(const std::vector<glm::vec4>& rTangents)
	{
		mTangents = rTangents;
	}

	void CalculateTangents();

	inline unsigned int GetNumberOfTriangles() const
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

	inline const std::vector<unsigned int> GetIndices() const
	{
		return mIndices;
	}
	
	void DrawCall();
	void AllocateResources();
	void DeallocateResources();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mUVs;
	std::vector<glm::vec4> mTangents;
	unsigned int mTriangleMeshVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mNormalsVBOId;
	unsigned int mUVsVBOId;
	unsigned int mTangentsVBOId;
	bool mUseTangents;

};

#endif
