#ifndef FASTCG_STANDARD_GEOMETRIES_H_
#define FASTCG_STANDARD_GEOMETRIES_H_

#include <FastCG/Mesh.h>

#include <glm/glm.hpp>

#include <memory>
#include <cstdint>

namespace FastCG
{
	class StandardGeometries
	{
	public:
		static std::unique_ptr<Mesh> CreateXYPlane(float width, float height, uint32_t xSegments = 1, uint32_t ySegments = 1, const glm::vec3& rCenter = glm::vec3(0.0f, 0.0f, 0.0f));
		static std::unique_ptr<Mesh> CreateXZPlane(float width, float depth, uint32_t xSegments = 1, uint32_t zSegments = 1, const glm::vec3& rCenter = glm::vec3(0.0f, 0.0f, 0.0f));
		static std::unique_ptr<Mesh> CreateSphere(float radius, uint32_t slices);

	private:
		StandardGeometries() = delete;
		~StandardGeometries() = delete;

	};

}

#endif
