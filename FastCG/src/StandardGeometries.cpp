#include <FastCG/StandardGeometries.h>
#include <FastCG/MathT.h>
#include <FastCG/Exception.h>

#include <vector>

namespace FastCG
{
	std::unique_ptr<Mesh> StandardGeometries::CreateXYPlane(const std::string &rName, float width, float height, uint32_t xSegments, uint32_t ySegments, const glm::vec3 &rCenter)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		float xIncrement = width / (float)xSegments;
		float yIncrement = height / (float)ySegments;
		glm::vec3 vScan = rCenter + glm::vec3(-(width * 0.5f), -(height * 0.5f), 0);

		size_t i = 0;
		for (uint32_t y = 0; y <= ySegments; y++)
		{
			glm::vec3 hScan = vScan;

			for (uint32_t x = 0; x <= xSegments; x++)
			{
				vertices.emplace_back(glm::vec3(hScan.x, hScan.y, hScan.z));
				normals.emplace_back(glm::vec3(0, 0, 1));
				uvs.emplace_back(glm::vec2(x / (float)xSegments, y / (float)ySegments));
				hScan += glm::vec3(xIncrement, 0, 0);
				i++;
			}

			vScan += glm::vec3(0, yIncrement, 0);
		}

		i = 0;
		std::vector<uint32_t> indexes((size_t)xSegments * ySegments * 6);

		for (uint32_t y = 1; y <= ySegments; y++)
		{
			for (uint32_t x = 0; x < xSegments; x++)
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

		auto *pMesh = new Mesh(rName, vertices, normals, uvs, indexes);
		pMesh->CalculateTangents();
		return std::unique_ptr<Mesh>(pMesh);
	}

	std::unique_ptr<Mesh> StandardGeometries::CreateXZPlane(const std::string &rName, float width, float depth, uint32_t xSegments, uint32_t zSegments, const glm::vec3 &rCenter)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		float xIncrement = width / (float)xSegments;
		float zIncrement = depth / (float)zSegments;
		int i = 0;
		glm::vec3 vScan = rCenter + glm::vec3(-(width * 0.5f), 0, -(depth * 0.5f));

		for (uint32_t z = 0; z <= zSegments; z++)
		{
			glm::vec3 hScan = vScan;

			for (uint32_t x = 0; x <= xSegments; x++)
			{
				vertices.emplace_back(glm::vec3(hScan.x, hScan.y, hScan.z));
				normals.emplace_back(glm::vec3(0, 1, 0));
				uvs.emplace_back(glm::vec2(x / (float)xSegments, z / (float)zSegments));
				hScan += glm::vec3(xIncrement, 0, 0);
				i++;
			}

			vScan += glm::vec3(0, 0, zIncrement);
		}

		i = 0;
		std::vector<uint32_t> indexes((size_t)xSegments * zSegments * 6);

		for (uint32_t z = 1; z <= zSegments; z++)
		{
			for (uint32_t x = 0; x < xSegments; x++)
			{
				auto i0 = (z * (xSegments + 1)) + x;
				auto i1 = ((z - 1) * (xSegments + 1)) + x;
				auto i2 = i1 + 1;
				auto i3 = i0 + 1;
				indexes[i++] = i2;
				indexes[i++] = i1;
				indexes[i++] = i0;
				indexes[i++] = i2;
				indexes[i++] = i0;
				indexes[i++] = i3;
			}
		}

		auto *pMesh = new Mesh(rName, vertices, normals, uvs, indexes);
		pMesh->CalculateTangents();
		return std::unique_ptr<Mesh>(pMesh);
	}

	std::unique_ptr<Mesh> StandardGeometries::CreateSphere(const std::string &rName, float radius, uint32_t slices)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		auto uStep = 2 * MathF::PI / (slices - 1);
		auto vStep = MathF::PI / (slices - 1);
		auto u = 0.0f;
		for (uint32_t i = 0; i < slices; i++)
		{
			auto cosU = MathF::Cos(u);
			auto sinU = MathF::Sin(u);

			auto v = 0.0f;
			for (uint32_t j = 0; j < slices; j++)
			{
				auto cosV = MathF::Cos(v);
				auto sinV = MathF::Sin(v);

				auto nx = sinV * cosU;
				auto ny = -cosV;
				auto nz = -sinV * sinU;

				auto n = MathF::Sqrt(nx * nx + ny * ny + nz * nz);

				if (n < 0.99f || n > 1.01f)
				{
					nx = nx / n;
					ny = ny / n;
					nz = nz / n;
				}

				auto tx = nz;
				auto ty = 0.0f;
				auto tz = -nx;

				auto a = MathF::Sqrt(tx * tx + ty * ty + tz * tz);

				if (a > 0.001f)
				{
					tx = tx / a;
					ty = ty / a;
					tz = tz / a;
				}

				auto x = radius * nx;
				auto y = radius * ny;
				auto z = radius * nz;

				vertices.emplace_back(glm::vec3(x, y, z));
				normals.emplace_back(glm::vec3(nx, ny, nz));
				uvs.emplace_back(glm::vec2(u / (2 * MathF::PI), v / MathF::PI));

				v += vStep;
			}
			u += uStep;
		}

		std::vector<uint32_t> indices;
		for (uint32_t i = 0; i < slices - 1; i++)
		{
			for (uint32_t j = 0; j < slices - 1; j++)
			{
				uint32_t p = i * slices + j;
				indices.emplace_back(p);
				indices.emplace_back(p + slices);
				indices.emplace_back(p + slices + 1);
				indices.emplace_back(p);
				indices.emplace_back(p + slices + 1);
				indices.emplace_back(p + 1);
			}
		}

		auto *pMesh = new Mesh(rName, vertices, normals, uvs, indices);
		pMesh->CalculateTangents();
		return std::unique_ptr<Mesh>(pMesh);
	}

}