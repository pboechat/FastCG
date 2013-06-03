#ifndef STANDARDGEOMETRIES_H_
#define STANDARDGEOMETRIES_H_

#include <Mesh.h>

#include <glm/glm.hpp>

class StandardGeometries
{
public:
	static Mesh* CreateXYPlane(float width, float height, unsigned int xSegments = 1, unsigned int ySegments = 1, const glm::vec3& rCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	static Mesh* CreateXZPlane(float width, float depth, unsigned int xSegments = 1, unsigned int zSegments = 1, const glm::vec3& rCenter = glm::vec3(0.0f, 0.0f, 0.0f));
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
