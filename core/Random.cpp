#include "include/Random.h"

#include <cstdlib>
#include <ctime>

Random::Random()
{
}

Random::~Random()
{
}

float Random::NextFloat()
{
	return rand() / (float) RAND_MAX;
}

void Random::Seed()
{
	srand((unsigned int) time(0));
}