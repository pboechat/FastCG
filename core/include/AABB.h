#ifndef AABB_H_
#define AABB_H_

#include <glm/glm.hpp>

struct AABB
{
	glm::vec3 min; 
	glm::vec3 max; 
	glm::vec3 center;

};

#endif