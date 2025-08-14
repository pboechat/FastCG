#ifndef FASTCG_STANDARD_GEOMETRIES_H
#define FASTCG_STANDARD_GEOMETRIES_H

#include <FastCG/Rendering/Mesh.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace FastCG
{
    class StandardGeometries
    {
    public:
        inline static std::unique_ptr<Mesh> CreateXYPlane(const std::string &rName, float width, float height,
                                                          uint32_t xSegments = 1, uint32_t ySegments = 1,
                                                          const glm::vec3 &rCenter = glm::vec3(0.0f, 0.0f, 0.0f));
        inline static std::unique_ptr<Mesh> CreateXZPlane(const std::string &rName, float width, float depth,
                                                          uint32_t xSegments = 1, uint32_t zSegments = 1,
                                                          const glm::vec3 &rCenter = glm::vec3(0.0f, 0.0f, 0.0f));
        inline static std::unique_ptr<Mesh> CreateSphere(const std::string &rName, float radius, uint32_t slices);

    private:
        StandardGeometries() = delete;
        ~StandardGeometries() = delete;
    };

}

#include <FastCG/Rendering/StandardGeometries.inc>

#endif
