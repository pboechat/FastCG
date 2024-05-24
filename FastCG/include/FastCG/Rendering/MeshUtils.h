#ifndef FASTCG_MESH_UTILS_H
#define FASTCG_MESH_UTILS_H

#include <FastCG/Core/AABB.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>

namespace FastCG
{
    class MeshUtils
    {
    public:
        inline static std::vector<glm::vec3> CalculateNormals(const std::vector<glm::vec3> &positions,
                                                              const std::vector<uint32_t> &indices);
        inline static std::vector<glm::vec4> CalculateTangents(const std::vector<glm::vec3> &positions,
                                                               const std::vector<glm::vec3> &normals,
                                                               const std::vector<glm::vec2> &uvs,
                                                               const std::vector<uint32_t> &indices);
        inline static AABB CalculateBounds(const std::vector<glm::vec3> &positions);

    private:
        MeshUtils() = delete;
        ~MeshUtils() = delete;
    };

}

#include <FastCG/Rendering/MeshUtils.inc>

#endif