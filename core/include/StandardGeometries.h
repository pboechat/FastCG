#ifndef STANDARDGEOMETRIES_H_
#define STANDARDGEOMETRIES_H_

#include <Mesh.h>

#include <glm/glm.hpp>

class StandardGeometries
{
public:
	static Mesh* CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int ySegments, const glm::vec3& rCenter);
	static Mesh* CreateXZPlane(float width, float depth, unsigned int xSegments, unsigned int zSegments, const glm::vec3& rCenter);
	static Mesh* CreateSphere(float radius, unsigned int slices);

private:
	StandardGeometries()
	{
	}

	virtual ~StandardGeometries()
	{
	}
};

#endif
