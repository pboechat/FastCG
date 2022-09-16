#ifndef FASTCG_MATERIAL_GROUPS_BATCHING_STRATEGY_H_
#define FASTCG_MATERIAL_GROUPS_BATCHING_STRATEGY_H_

#include <FastCG/RenderBatchingStrategy.h>

namespace FastCG
{
	class MaterialGroupsBatchingStrategy : public RenderBatchingStrategy
	{
	public:
		MaterialGroupsBatchingStrategy(std::vector<std::unique_ptr<RenderBatch>>& rRenderBatches) :
			RenderBatchingStrategy(rRenderBatches)
		{
		}

		void AddMeshFilter(MeshFilter* pMeshFilter) override;
		void RemoveMeshFilter(MeshFilter* pMeshFilter) override;

	};

}

#endif