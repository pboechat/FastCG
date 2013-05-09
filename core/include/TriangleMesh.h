#ifndef TRIANGLEMESH_H_
#define TRIANGLEMESH_H_

#include <Transformable.h>
#include <Drawable.h>
#include <Pointer.h>
#include <Material.h>

#include <glm/glm.hpp>

#include <vector>

class TriangleMesh : public Transformable, public Drawable
{
public:
	TriangleMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const MaterialPtr& materialPtr);
#ifdef USE_PROGRAMMABLE_PIPELINE
	TriangleMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& tangents, const std::vector<glm::vec2>& uvs, const MaterialPtr& materialPtr);
#endif
	virtual ~TriangleMesh();

	inline MaterialPtr GetMaterial() const
	{
		return mMaterialPtr;
	}

	inline void SetMaterial(const MaterialPtr& materialPtr)
	{
		mMaterialPtr = materialPtr;
	}

#ifdef USE_PROGRAMMABLE_PIPELINE
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

protected:
	virtual void OnDraw();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndexes;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mUVs;
	MaterialPtr mMaterialPtr;
#ifdef USE_PROGRAMMABLE_PIPELINE
	std::vector<glm::vec4> mTangents;
	unsigned int mTriangleMeshVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mNormalsVBOId;
	unsigned int mUVsVBOId;
	unsigned int mTangentsVBOId;
	bool mUseTangents;
#else
	unsigned int mDisplayListId;
#endif

	void AllocateResources();
	void DeallocateResources();

};

typedef Pointer<TriangleMesh> TriangleMeshPtr;

#endif
