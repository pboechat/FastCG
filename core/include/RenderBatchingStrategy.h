#ifndef RENDERBATCHINGSTRATEGY_H_
#define RENDERBATCHINGSTRATEGY_H_

#include <RenderBatch.h>
#include <MeshFilter.h>

#include <vector>

class RenderBatchingStrategy
{
public:
	RenderBatchingStrategy(std::vector<RenderBatch*>& rRenderBatches) :
		mrRenderBatches(rRenderBatches)
	{
	}

	virtual void AddMeshFilter(MeshFilter* pMeshFilter) = 0;
	virtual void RemoveMeshFilter(MeshFilter* pMeshFilter) = 0;

protected:
	std::vector<RenderBatch*>& mrRenderBatches;

};

#endif