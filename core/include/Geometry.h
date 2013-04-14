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
	static const unsigned int VERTICES_ATTRIBUTE_INDEX = 0;
	static const unsigned int NORMALS_ATTRIBUTE_INDEX = 1;
	static const unsigned int UVS_ATTRIBUTE_INDEX = 2;

	Geometry(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, MaterialPtr materialPtr);
	virtual ~Geometry();

	inline void SetMaterial(MaterialPtr materialPtr)
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
