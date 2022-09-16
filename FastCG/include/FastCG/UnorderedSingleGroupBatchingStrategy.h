#ifndef FASTCG_UNORDERED_SINGLE_BATCHING_STRATEGY_H_
#define FASTCG_UNORDERED_SINGLE_BATCHING_STRATEGY_H_

#include <FastCG/RenderBatchingStrategy.h>

namespace FastCG
{
	class UnorderedSingleGroupBatchingStrategy : public RenderBatchingStrategy
	{
	public:
		UnorderedSingleGroupBatchingStrategy(std::vector<std::unique_ptr<RenderBatch>>& rRenderBatches) :
			RenderBatchingStrategy(rRenderBatches)
		{
		}

		void AddMeshFilter(MeshFilter* pMeshFilter) override;
		void RemoveMeshFilter(MeshFilter* pMeshFilter) override;

	};

}

#endif