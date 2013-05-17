#include <Random.h>

#include <cstdlib>
#include <ctime>


float Random::NextFloat()
{
	return rand() / (float) RAND_MAX;
}

glm::vec4 Random::NextColor()
{
	return glm::vec4(NextFloat(), NextFloat(), NextFloat(), 1.0f);
}

void Random::SeedWithTime()
{
	srand((unsigned int) time(0));
}