#include <StandardGeometries.h>
#include <Exception.h>
#include <MathF.h>

#include <vector>

TriangleMeshPtr StandardGeometries::CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int ySegments, const glm::vec3& rCenter, const MaterialPtr& materialPtr)
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
			normals.push_back(glm::vec3(0, 0, 1));
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
			indexes[i++] = i1;
			indexes[i++] = i2;
			indexes[i++] = i3;
			indexes[i++] = i1;
			indexes[i++] = i3;
			indexes[i++] = i0;
		}
	}

	return new TriangleMesh(vertices, indexes, normals, uvs, materialPtr);
}

TriangleMeshPtr StandardGeometries::CreateXZPlane(float width, float depth, unsigned int xSegments, unsigned int zSegments, const glm::vec3& rCenter, const MaterialPtr& materialPtr)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	float xIncrement = width / (float) xSegments;
	float zIncrement = depth / (float) zSegments;
	int i = 0;
	glm::vec3 vScan = rCenter + glm::vec3(-(width * 0.5f), 0, -(depth * 0.5f));

	for (unsigned int z = 0; z <= zSegments; z++)
	{
		glm::vec3 hScan = vScan;

		for (unsigned int x = 0; x <= xSegments; x++)
		{
			vertices.push_back(glm::vec3(hScan.x, hScan.y, hScan.z));
			normals.push_back(glm::vec3(0, 1, 0));
			uvs.push_back(glm::vec2(x / (float) xSegments, z / (float) zSegments));
			hScan += glm::vec3(xIncrement, 0, 0);
			i++;
		}

		vScan += glm::vec3(0, 0, zIncrement);
	}

	i = 0;
	std::vector<unsigned int> indexes((xSegments * zSegments * 2) * 3);

	for (unsigned int z = 1; z <= zSegments; z++)
	{
		for (unsigned int x = 0; x < xSegments; x++)
		{
			int i0 = (z * (xSegments + 1)) + x;
			int i1 = ((z - 1) * (xSegments + 1)) + x;
			int i2 = i1 + 1;
			int i3 = i0 + 1;
			indexes[i++] = i1;
			indexes[i++] = i2;
			indexes[i++] = i3;
			indexes[i++] = i1;
			indexes[i++] = i3;
			indexes[i++] = i0;
		}
	}

	return new TriangleMesh(vertices, indexes, normals, uvs, materialPtr);
}

TriangleMeshPtr StandardGeometries::CreateSphere(float radius, unsigned int zSegments, unsigned int radialSegments, const MaterialPtr& materialPtr)
{
	unsigned int vertexQuantity = (zSegments - 2) * (radialSegments + 1) + 2;
	std::vector<glm::vec3> vertices(vertexQuantity);
	std::vector<glm::vec3> normals(vertexQuantity);
	std::vector<glm::vec2> uvs(vertexQuantity);

	float radialFactor = 1.0f / (radialSegments);
	float zFactor = 2.0f / (zSegments - 1);

	// Generate points on the unit circle to be used in computing the mesh
	// points on a cylinder slice.
	float* pSines = new float[radialSegments + 1];
	float* pCossines = new float[radialSegments + 1];

	for (unsigned int radialIndex = 0; radialIndex < radialSegments; radialIndex++)
	{
		float angle = (2.0f * MathF::PI) * radialFactor * radialIndex;
		pCossines[radialIndex] = MathF::Cos(angle);
		pSines[radialIndex] = MathF::Sin(angle);
	}

	pSines[radialSegments] = pSines[0];
	pCossines[radialSegments] = pCossines[0];

	unsigned int i = 0;
	for (unsigned int zIndex = 1; zIndex < zSegments - 1; zIndex++)
	{
		float zFraction = -1.0f + zFactor * zIndex;  // in (-1,1)
		float z = radius * zFraction;
		// compute center of slice
		glm::vec3 sliceCenter(0.0f, 0.0f, z);
		// compute radius of slice
		float sliceRadius = MathF::Sqrt(MathF::Abs(radius * radius - z * z));
		// compute slice vertices with duplication at end point
		int save = i;

		for (unsigned int r = 0; r < radialSegments; r++)
		{
			float radialFraction = r * radialFactor;  // in [0,1)
			glm::vec3 radial(pCossines[r], pSines[r], 0.0F);
			vertices[i] = sliceCenter + sliceRadius * radial;
			normals[i] = glm::normalize(vertices[i]);
			uvs[i] = glm::vec2(radialFraction, 0.5f * (zFraction + 1.0f));
			i++;
		}

		vertices[i] = vertices[save];
		normals[i] = normals[save];
		uvs[i] = glm::vec2(1.0f, 0.5f * (zFraction + 1.0f));
		i++;
	}

	// south pole
	vertices[i] = glm::vec3(0.0f, 0.0f, -radius);
	normals[i] = glm::vec3(0.0f, 0.0f, -1.0f);
	uvs[i] = glm::vec2(0.5f, 0.5f);
	i++;

	// north pole
	vertices[i] = glm::vec3(0.0f, 0.0f, radius);
	normals[i] = glm::vec3(0.0f, 0.0f, 1.0f);
	uvs[i] = glm::vec2(0.5f, 1.0f);
	i++;

	if (i != vertexQuantity)
	{
		THROW_EXCEPTION(Exception, "Invalid vertex quantity: %d", vertexQuantity);
	}

	if (!(radialSegments < (32768 - 1) && (radialSegments >= 0)))
	{
		THROW_EXCEPTION(Exception, "Invalid radial segments: %d", radialSegments);
	}

	unsigned int radialSegmentsCount = radialSegments;

	if (!(vertexQuantity < (32768) && (vertexQuantity >= 0)))
	{
		THROW_EXCEPTION(Exception, "Invalid vertex quantity: %d", vertexQuantity);
	}

	if (!(zSegments < (32768) && (zSegments >= 0)))
	{
		THROW_EXCEPTION(Exception, "Invalid z segments: %d", zSegments);
	}

	unsigned int zSegmentsCount = zSegments;
	unsigned int triangleQuantity = 2 * (zSegments - 2) * radialSegments;
	std::vector<unsigned int> indexes(3 * triangleQuantity);

	// generate connectivity
	unsigned int zStart = 0;
	i = 0;

	for (unsigned int z = 0; z < zSegments - 3; z++)
	{
		unsigned int i0 = zStart;
		unsigned int i1 = i0 + 1;
		zStart += (radialSegmentsCount + 1);
		unsigned int i2 = zStart;
		unsigned int i3 = i2 + 1;

		for (unsigned int j = 0; j < radialSegments; j++)
		{
			indexes[i] = i0++;
			indexes[i + 1] = i1;
			indexes[i + 2] = i2;
			indexes[i + 3] = i1++;
			indexes[i + 4] = i3++;
			indexes[i + 5] = i2++;
			i += 6;
		}
	}

	// south pole triangles

	for (unsigned int j = 0; j < radialSegmentsCount; j++)
	{
		indexes[i] = j;
		indexes[i + 1] = vertexQuantity - 2;
		indexes[i + 2] = j + 1;
		i += 3;
	}

	// north pole triangles
	unsigned int offset = (zSegmentsCount - 3) * (radialSegmentsCount + 1);
	for (unsigned int j = 0; j < radialSegmentsCount; j++)
	{
		indexes[i] = j + offset;
		indexes[i + 1] = j + 1 + offset;
		indexes[i + 2] = vertexQuantity - 1;
		i += 3;
	}

	delete[] pSines;
	delete[] pCossines;

	if (indexes.size() != 3 * triangleQuantity)
	{
		THROW_EXCEPTION(Exception, "Invalid triangle quantity: %d", triangleQuantity);
	}

	return new TriangleMesh(vertices, indexes, normals, uvs, materialPtr);
}

