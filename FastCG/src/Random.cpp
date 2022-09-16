#include <FastCG/Random.h>

#include <cstdlib>
#include <ctime>

namespace FastCG
{
	float Random::NextFloat()
	{
		return rand() / (float)RAND_MAX;
	}

	int Random::Range(int min, int max)
	{
		return min + (int)(NextFloat() * (max - min));
	}

	glm::vec4 Random::NextColor()
	{
		return glm::vec4(NextFloat(), NextFloat(), NextFloat(), 1.0f);
	}

	void Random::SeedWithTime()
	{
		srand((unsigned int)time(0));
	}

}