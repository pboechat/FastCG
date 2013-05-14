#include <StandardGeometries.h>
#include <Exception.h>
#include <MathT.h>

#include <vector>

Mesh* StandardGeometries::CreateXYPlane(float width, float height, unsigned int xSegments, unsigned int ySegments, const glm::vec3& rCenter)
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

	Mesh* pMesh = new Mesh(vertices, indexes, normals, uvs);
#ifdef USE_PROGRAMMABLE_PIPELINE
	pMesh->CalculateTangents();
#endif
	return pMesh;
}

Mesh* StandardGeometries::CreateXZPlane(float width, float depth, unsigned int xSegments, unsigned int zSegments, const glm::vec3& rCenter)
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

	Mesh* pMesh = new Mesh(vertices, indexes, normals, uvs);
#ifdef USE_PROGRAMMABLE_PIPELINE
	pMesh->CalculateTangents();
#endif
	return pMesh;
}

Mesh* StandardGeometries::CreateSphere(float radius, unsigned int slices)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
#ifdef USE_PROGRAMMABLE_PIPELINE
	std::vector<glm::vec4> tangents;
#endif
	std::vector<glm::vec2> uvs;

	float u_step = 2 * MathF::PI / (slices - 1);
	float v_step = MathF::PI / (slices - 1);
	float u = 0.0f;
	for (unsigned int i = 0; i < slices; i++)
	{
		float cos_u = MathF::Cos(u);
		float sin_u = MathF::Sin(u);

		float v = 0.0f;
		for (unsigned int j = 0; j < slices; j++)
		{
			float cos_v = MathF::Cos(v);
			float sin_v = MathF::Sin(v);

			float nx = sin_v * cos_u;
			float ny = -cos_v;
			float nz = -sin_v * sin_u;

			float n = MathF::Sqrt( nx * nx + ny * ny + nz * nz );

			if (n < 0.99f || n > 1.01f)
			{
				nx = nx / n;
				ny = ny / n;
				nz = nz / n;
			}
			
			float tx = nz;
			float ty = 0;
			float tz = -nx;

			float a = MathF::Sqrt( tx * tx + ty * ty + tz * tz );

			if (a > 0.001f) 
			{
				tx = tx / a;
				ty = ty / a;
				tz = tz / a;
			}

			float x = radius * nx;
			float y = radius * ny;
			float z = radius * nz;

			vertices.push_back(glm::vec3(x, y, z));
			normals.push_back(glm::vec3(nx, ny, nz));
#ifdef USE_PROGRAMMABLE_PIPELINE
			tangents.push_back(glm::vec4(MathF::Round(255.0f * (0.5f - 0.5f * tx)), MathF::Round(255.0f * (0.5f - 0.5f * ty)), MathF::Round(255.0f * (0.5f - 0.5f * tz)), 1.0f));
#endif
			uvs.push_back(glm::vec2(u / (2 * MathF::PI), v / MathF::PI));

			v += v_step;
		}
		u += u_step;
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < slices - 1; i++)
	{
		for (unsigned j = 0; j < slices - 1; j++)
		{
			unsigned int p = i * slices + j;
			indices.push_back(p);
			indices.push_back(p + slices);
			indices.push_back(p + slices + 1);
			indices.push_back(p);
			indices.push_back(p + slices + 1);
			indices.push_back(p + 1);
		}
	}

#ifdef USE_PROGRAMMABLE_PIPELINE
	return new Mesh(vertices, indices, normals, tangents, uvs);
#else
	return new Mesh(vertices, indices, normals, uvs);
#endif
}
