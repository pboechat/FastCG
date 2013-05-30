#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
	Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs);
#ifndef FIXED_FUNCTION_PIPELINE
	Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& tangents, const std::vector<glm::vec2>& uvs);
#endif
	virtual ~Mesh();

#ifndef FIXED_FUNCTION_PIPELINE
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
#endif

	inline unsigned int GetNumberOfTriangles() const
	{
		return mIndexes.size() / 3;
	}
	
	void DrawCall();
	void AllocateResources();
	void DeallocateResources();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndexes;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mUVs;
#ifdef FIXED_FUNCTION_PIPELINE
	unsigned int mDisplayListId;
#else
	std::vector<glm::vec4> mTangents;
	unsigned int mTriangleMeshVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mNormalsVBOId;
	unsigned int mUVsVBOId;
	unsigned int mTangentsVBOId;
	bool mUseTangents;
#endif

};

#endif
