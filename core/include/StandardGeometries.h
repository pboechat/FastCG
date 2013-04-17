#ifndef STANDARDGEOMETRIES_H_
#define STANDARDGEOMETRIES_H_

#include <Geometry.h>
#include <Material.h>

#include <glm/glm.hpp>

class StandardGeometries
{
public:
	static GeometryPtr CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int ySegments, const glm::vec3& rCenter, const MaterialPtr& materialPtr);
	static GeometryPtr CreateXZPlane(float width, float depth, unsigned int xSegments, unsigned int zSegments, const glm::vec3& rCenter, const MaterialPtr& materialPtr);
	static GeometryPtr CreateSphere (float radius, unsigned int zSegments, unsigned int radialSegments, const MaterialPtr& materialPtr);

private:
	StandardGeometries()
	{
	}

	virtual ~StandardGeometries()
	{
	}
};

#endif
