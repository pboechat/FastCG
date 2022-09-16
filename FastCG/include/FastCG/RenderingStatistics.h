#ifndef FASTCG_RENDERING_STATISTICS_H_
#define FASTCG_RENDERING_STATISTICS_H_

namespace FastCG
{
	struct RenderingStatistics
	{
		size_t drawCalls{ 0 };
		size_t numberOfTriangles{ 0 };

		void Reset()
		{
			drawCalls = 0;
			numberOfTriangles = 0;
		}

	};

}

#endif