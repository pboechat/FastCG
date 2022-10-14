#ifndef FASTCG_RENDERING_STATISTICS_H
#define FASTCG_RENDERING_STATISTICS_H

namespace FastCG
{
	struct RenderingStatistics
	{
		size_t drawCalls{0};
		size_t numberOfTriangles{0};

		void Reset()
		{
			drawCalls = 0;
			numberOfTriangles = 0;
		}
	};

}

#endif