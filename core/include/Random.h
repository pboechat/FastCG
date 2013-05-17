#ifndef RANDOM_H_
#define RANDOM_H_

#include <glm/glm.hpp>

class Random
{
public:
	static float NextFloat();
	static glm::vec4 NextColor();
	static void SeedWithTime();

private:
	Random()
	{
	}

	~Random()
	{
	}

};

#endif
