#ifndef FASTCG_TIMER_H_
#define FASTCG_TIMER_H_

#include <FastCG/Exception.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace FastCG
{
	class Timer
	{
	public:
		Timer()
		{
#ifdef _WIN32
			LARGE_INTEGER frequency;

			if (QueryPerformanceFrequency(&frequency))
			{
				mSeconds = 1.0 / frequency.QuadPart;
			}
			else
			{
				FASTCG_THROW_EXCEPTION(Exception, "Cannot query performance counter frequency: %d", 0);
			}
#else
#error "FastCG::Timer::Timer() is not implemented on the current platform"
#endif
		}

		inline double GetTime()
		{
#ifdef _WIN32
			LARGE_INTEGER time;

			if (QueryPerformanceCounter(&time))
			{
				return (double)time.QuadPart * mSeconds;
			}
			else
			{
				FASTCG_THROW_EXCEPTION(Exception, "Cannot query performance counter: %d", 0);
				return 0;
			}
#else
#error "FastCG::Timer::GetTime() is not implemented on the current platform"
#endif
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
		double mStart{0};
		double mEnd{0};
		double mSeconds{0};
	};

}

#endif
