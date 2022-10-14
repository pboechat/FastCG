#ifndef FASTCG_RENDER_BATCHING_STRATEGY_H_
#define FASTCG_RENDER_BATCHING_STRATEGY_H_

#include <FastCG/RenderBatch.h>
#include <FastCG/MeshFilter.h>

#include <vector>
#include <memory>

namespace FastCG
{
	class RenderBatchingStrategy
	{
	public:
		RenderBatchingStrategy(std::vector<std::unique_ptr<RenderBatch>> &rRenderBatches) : mrRenderBatches(rRenderBatches)
		{
		}

		virtual void AddMeshFilter(MeshFilter *pMeshFilter) = 0;
		virtual void RemoveMeshFilter(MeshFilter *pMeshFilter) = 0;

	protected:
		std::vector<std::unique_ptr<RenderBatch>> &mrRenderBatches;
	};

}

#endif