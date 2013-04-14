#include <StandardGeometries.h>

#include <vector>

GeometryPtr StandardGeometries::CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int ySegments, const glm::vec3& rCenter, MaterialPtr materialPtr)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	float xIncrement = width / (float) xSegments;
	float yIncrement = height / (float) ySegments;

	int i = 0;
	glm::vec3 vScan = rCenter + glm::vec3(-(width * 0.5f), -(height * 0.5f), 0);
	for (unsigned int y = 0; y <= ySegments; y++)
	{
		glm::vec3 hScan = vScan;
		for (unsigned int x = 0; x <= xSegments; x++)
		{
			vertices.push_back(glm::vec3(hScan.x, hScan.y, hScan.z));
			normals.push_back(glm::vec3(0, 1, 0));
			uvs.push_back(glm::vec2(x / (float) xSegments, y / (float) ySegments));
			hScan += glm::vec3(xIncrement, 0, 0);
			i++;
		}
		vScan += glm::vec3(0, yIncrement, 0);
	}

	i = 0;
	std::vector<unsigned int> indexes((xSegments * ySegments * 2) * 3);
	for (unsigned int y = 1; y <= ySegments; y++)
	{
		for (unsigned int x = 0; x < xSegments; x++)
		{
			int i0 = (y * (xSegments + 1)) + x;
			int i1 = ((y - 1) * (xSegments + 1)) + x;
			int i2 = i1 + 1;
			int i3 = i0 + 1;

			indexes[i++] = i2;
			indexes[i++] = i1;
			indexes[i++] = i0;

			indexes[i++] = i2;
			indexes[i++] = i0;
			indexes[i++] = i3;
		}
	}

	return new Geometry(vertices, indexes, normals, uvs, materialPtr);
}

