#ifndef THREAD_H_
#define THREAD_H_

#ifdef _WIN32
#include <Windows.h>
#endif

class Thread
{
public:
	static void Sleep(double seconds)
	{
#ifdef _WIN32
		SleepEx((long)seconds * 1000L, 0);
#endif
	}

};

#endif