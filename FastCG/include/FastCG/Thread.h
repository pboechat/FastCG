#ifndef FASTCG_THREAD_H
#define FASTCG_THREAD_H

#include <FastCG/FastCG.h>

#ifdef FASTCG_WINDOWS
#include <Windows.h>
#endif

namespace FastCG
{
	class Thread
	{
	public:
		static void Sleep(double seconds)
		{
#ifdef FASTCG_WINDOWS
			SleepEx((long)(seconds * 1000.0), 0);
#else
#error "Thread::Sleep() not implemented on current platform"
#endif
		}
	};

}

#endif