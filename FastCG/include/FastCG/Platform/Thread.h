#ifndef FASTCG_THREAD_H
#define FASTCG_THREAD_H

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_POSIX
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
			::Sleep(static_cast<DWORD>(seconds * 1e3));
#elif defined FASTCG_POSIX
			sleep((unsigned int)seconds);
#else
#error "Thread::Sleep() not implemented on current platform"
#endif
		}
	};

}

#endif