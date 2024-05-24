#include <cstdlib>
#include <ctime>

namespace FastCG
{
    float Random::NextFloat()
    {
        return rand() / (float)RAND_MAX;
    }

    glm::vec4 Random::NextColor()
    {
        return glm::vec4(NextFloat(), NextFloat(), NextFloat(), 1.0f);
    }

    void Random::Seed(unsigned int seed)
    {
        srand(seed);
    }

    void Random::SeedWithTime()
    {
        Seed((unsigned int)time(0));
    }

}