#ifndef FASTCG_RANDOM_H_
#define FASTCG_RANDOM_H_

#include <glm/glm.hpp>

namespace FastCG
{
	class Random
	{
	public:
		static float NextFloat();
		static int Range(int min, int max);
		static glm::vec4 NextColor();
		static void Seed(unsigned int seed);
		static void SeedWithTime();

	private:
		Random() = delete;
		~Random() = delete;

	};

}

#endif
