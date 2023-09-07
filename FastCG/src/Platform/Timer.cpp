#include <FastCG/Platform/Timer.h>

namespace FastCG
{
#if defined FASTCG_WINDOWS
    double Timer::sFrequency = -1;
#endif

}