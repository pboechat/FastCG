#ifndef FASTCG_RANDOM_H_
#define FASTCG_RANDOM_H_

#include <glm/glm.hpp>

namespace FastCG
{
	class Random
	{
	public:
		static float NextFloat();
		template<typename T>
		static T Range(T min, T max)
		{
			return min + (int)(NextFloat() * (max - min));
		}
		static glm::vec4 NextColor();
		static void Seed(unsigned int seed);
		static void SeedWithTime();

	private:
		Random() = delete;
		~Random() = delete;

	};

}

#endif
