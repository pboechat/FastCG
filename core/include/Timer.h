#ifndef TIMER_H
#define TIMER_H

#include <Windows.h>

class Timer
{
public:
	Timer() : mStart(0), mEnd(0), mMilliseconds(0)
	{
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency))
		{
			mMilliseconds = 1000.0 / frequency.QuadPart;
		}
		else
		{
			// TODO:
			throw Exception("Cannot query performance counter frequency!");
		}
	}

	~Timer() {}

	inline double GetTime()
	{
		LARGE_INTEGER time;
		if (QueryPerformanceCounter(&time))
		{
			return time.QuadPart * mMilliseconds;
		}
		else
		{
			// TODO:
			throw Exception("Cannot query performance counter!");
		}
	}

	inline void Start()
	{
		mStart = GetTime();
	}

	inline void End()
	{
		mEnd = GetTime();
	}

	inline double GetElapsedTime() const
	{
		return (mEnd - mStart);
	}

private:
	double mStart;
	double mEnd;
	double mMilliseconds;

};

#endif