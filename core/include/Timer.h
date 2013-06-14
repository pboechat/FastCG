#ifndef TIMER_H_
#define TIMER_H_

#include <Exception.h>

#include <Windows.h>

class Timer
{
public:
	Timer() :
		mStart(0), mEnd(0), mSeconds(0)
	{
		LARGE_INTEGER frequency;

		if (QueryPerformanceFrequency(&frequency))
		{
			mSeconds = 1.0 / frequency.QuadPart;
		}

		else
		{
			THROW_EXCEPTION(Exception, "Cannot query performance counter frequency: %d", 0);
		}
	}

	~Timer()
	{
	}

	inline double GetTime()
	{
		LARGE_INTEGER time;

		if (QueryPerformanceCounter(&time))
		{
			return (double)time.QuadPart * mSeconds;
		}

		else
		{
			THROW_EXCEPTION(Exception, "Cannot query performance counter: %d", 0);
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
	double mSeconds;

};

#endif
