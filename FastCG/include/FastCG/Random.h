#ifndef FASTCG_RANDOM_H
#define FASTCG_RANDOM_H

#include <glm/glm.hpp>

namespace FastCG
{
	class Random
	{
	public:
		inline static float NextFloat();
		template <typename T>
		inline static T Range(T min, T max)
		{
			return min + (int)(NextFloat() * (max - min));
		}
		inline static glm::vec4 NextColor();
		inline static void Seed(unsigned int seed);
		inline static void SeedWithTime();

	private:
		Random() = delete;
		~Random() = delete;
	};

}

#include <FastCG/Random.inc>

#endif
