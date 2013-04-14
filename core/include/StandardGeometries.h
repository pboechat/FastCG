#ifndef STANDARDGEOMETRIES_H_
#define STANDARDGEOMETRIES_H_

#include <Geometry.h>
#include <Material.h>

#include <glm/glm.hpp>

class StandardGeometries
{
public:
	static GeometryPtr CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int zSegments, const glm::vec3& rCenter, MaterialPtr materialPtr);

private:
	StandardGeometries()
	{
	}

	virtual ~StandardGeometries()
	{
	}
};

#endif
