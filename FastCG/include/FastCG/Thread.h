#ifndef FASTCG_THREAD_H
#define FASTCG_THREAD_H

#include <FastCG/FastCG.h>

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <unistd.h>
#endif

namespace FastCG
{
	class Thread
	{
	public:
		static void Sleep(double seconds)
		{
#if defined FASTCG_WINDOWS
			SleepEx((long)(seconds * 1000.0), 0);
#elif defined FASTCG_LINUX
			sleep((unsigned int)seconds);
#else
#error "Thread::Sleep() not implemented on current platform"
#endif
		}
	};

}

#endif