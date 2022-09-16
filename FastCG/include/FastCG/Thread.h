#ifndef FASTCG_THREAD_H_
#define FASTCG_THREAD_H_

#ifdef _WIN32
#include <Windows.h>
#endif

namespace FastCG
{
	class Thread
	{
	public:
		static void Sleep(double seconds)
		{
#ifdef _WIN32
			SleepEx((long)(seconds * 1000.0), 0);
#endif
		}

	};

}

#endif