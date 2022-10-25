#ifndef FASTCG_TIMER_H
#define FASTCG_TIMER_H

#include <FastCG/FastCG.h>
#include <FastCG/Exception.h>

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <time.h>
#endif

namespace FastCG
{
#if defined FASTCG_WINDOWS
	class Timer
	{
	public:
		Timer()
		{
			LARGE_INTEGER frequency;

			if (QueryPerformanceFrequency(&frequency))
			{
				mSeconds = 1.0 / frequency.QuadPart;
			}
			else
			{
				FASTCG_THROW_EXCEPTION(Exception, "Cannot query performance counter frequency: %d", 0);
			}
		}

		inline double GetTime() const
		{
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
#elif defined FASTCG_LINUX
	class Timer
	{
	public:
		inline double GetTime() const
		{
			timespec time;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
			return (double)time.tv_sec + time.tv_nsec * 1e-9;
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
	};
#else
#error "FastCG::Timer is not implemented on current platform"
#endif

}

#endif
