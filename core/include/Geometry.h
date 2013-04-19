#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <Transformable.h>
#include <Pointer.h>
#include <Material.h>

#include <vector>
#include <glm/glm.hpp>

class Geometry : public Transformable
{
public:
	Geometry(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const MaterialPtr& materialPtr);
	virtual ~Geometry();

	inline MaterialPtr GetMaterial() const
	{
		return mMaterialPtr;
	}

	inline void SetMaterial(const MaterialPtr& materialPtr)
	{
		mMaterialPtr = materialPtr;
	}

	void Draw();

#ifdef USE_OPENGL4
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

private:
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndexes;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mUVs;
	MaterialPtr mMaterialPtr;
#ifdef USE_OPENGL4
	std::vector<glm::vec4> mTangents;
	unsigned int mGeometryVAOId;
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

typedef Pointer<Geometry> GeometryPtr;

#endif
