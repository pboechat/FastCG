#ifndef RENDERINGSTATISTICS_H_
#define RENDERINGSTATISTICS_H_

struct RenderingStatistics
{
	int drawCalls;
	int numberOfTriangles;

	void Reset()
	{
		drawCalls = 0;
		numberOfTriangles = 0;
	}
};

#endif