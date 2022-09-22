#ifndef FASTCG_AABB_H_
#define FASTCG_AABB_H_

#include <glm/glm.hpp>

namespace FastCG
{
	struct AABB
	{
		glm::vec3 min{ 0, 0, 0 };
		glm::vec3 max{ 0, 0, 0 };

		inline glm::vec3 getExtent() const
		{
			return max - min;
		}

		inline glm::vec3 getCenter() const
		{
			return min + getExtent() * 0.5f;
		}

		void Expand(const AABB& other)
		{
			max = glm::max(max, other.max);
			min = glm::min(min, other.min);
		}

	};

}

#endif