#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Material.h"

#include <vector>
#include <glm/glm.hpp>

class Geometry
{
public:
	static const unsigned int VERTICES_ATTRIBUTE_INDEX = 0;
	static const unsigned int UVS_ATTRIBUTE_INDEX = 1;

	Geometry(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec2>& uvs, const Material& material);
	virtual ~Geometry();

	void Draw();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndexes;
	std::vector<glm::vec2> mUVs;
	Material mMaterial;
	unsigned int mGeometryVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mUVsVBOId;

	void AllocateResources();
	void DeallocateResources();

};

#endif
