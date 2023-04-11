#ifndef FASTCG_RENDERING_STATISTICS_H
#define FASTCG_RENDERING_STATISTICS_H

#include <cstdint>

namespace FastCG
{
	struct RenderingStatistics
	{
		uint32_t drawCalls{0};
		uint32_t triangles{0};

		void Reset()
		{
			drawCalls = 0;
			triangles = 0;
		}
	};

}

#endif