#ifndef FASTCG_SINGLE_BATCH_MESH_BATCHING_STRATEGY_H
#define FASTCG_SINGLE_BATCH_MESH_BATCHING_STRATEGY_H

#include <FastCG/MeshBatchingStrategy.h>

namespace FastCG
{
	class SingleBatchMeshBatchingStrategy : public MeshBatchingStrategy
	{
	public:
		SingleBatchMeshBatchingStrategy(std::vector<std::unique_ptr<MeshBatch>> &rMeshBatches) : MeshBatchingStrategy(rMeshBatches)
		{
		}

		void AddMeshFilter(MeshFilter *pMeshFilter) override;
		void RemoveMeshFilter(MeshFilter *pMeshFilter) override;
	};

}

#endif