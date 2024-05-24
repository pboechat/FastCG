#ifndef FASTCG_AABB_H
#define FASTCG_AABB_H

#include <glm/glm.hpp>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace FastCG
{
    struct AABB
    {
        glm::vec3 min{0, 0, 0};
        glm::vec3 max{0, 0, 0};

        inline glm::vec3 getExtent() const
        {
            return max - min;
        }

        inline glm::vec3 getCenter() const
        {
            return min + getExtent() * 0.5f;
        }

        inline void Expand(const AABB &other)
        {
            max = glm::max(max, other.max);
            min = glm::min(min, other.min);
        }

        inline bool IsDegenerated() const
        {
            return max.x <= min.x || max.y <= min.y || max.z <= min.z;
        }
    };

}

#endif