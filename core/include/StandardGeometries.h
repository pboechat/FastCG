#ifndef STANDARDGEOMETRIES_H_
#define STANDARDGEOMETRIES_H_

#include <TriangleMesh.h>
#include <Material.h>

#include <glm/glm.hpp>

class StandardGeometries
{
public:
	static TriangleMeshPtr CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int ySegments, const glm::vec3& rCenter, const MaterialPtr& materialPtr);
	static TriangleMeshPtr CreateXZPlane(float width, float depth, unsigned int xSegments, unsigned int zSegments, const glm::vec3& rCenter, const MaterialPtr& materialPtr);
	static TriangleMeshPtr CreateSphere(float radius, unsigned int zSegments, unsigned int radialSegments, const MaterialPtr& materialPtr);

private:
	StandardGeometries()
	{
	}

	virtual ~StandardGeometries()
	{
	}
};

#endif
