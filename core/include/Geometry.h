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

	inline void SetMaterial(const MaterialPtr& materialPtr)
	{
		mMaterialPtr = materialPtr;
	}

	void Draw();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndexes;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec2> mUVs;
	MaterialPtr mMaterialPtr;
	unsigned int mGeometryVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mIndexesVBOId;
	unsigned int mNormalsVBOId;
	unsigned int mUVsVBOId;

	void AllocateResources();
	void DeallocateResources();

};

typedef Pointer<Geometry> GeometryPtr;

#endif
