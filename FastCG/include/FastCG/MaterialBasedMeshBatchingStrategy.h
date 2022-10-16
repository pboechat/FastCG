#ifndef FASTCG_MATERIAL_GROUPS_MESH_BATCHING_STRATEGY_H
#define FASTCG_MATERIAL_GROUPS_MESH_BATCHING_STRATEGY_H

#include <FastCG/MeshBatchingStrategy.h>

namespace FastCG
{
	class MaterialBasedMeshBatchingStrategy : public MeshBatchingStrategy
	{
	public:
		MaterialBasedMeshBatchingStrategy(std::vector<std::unique_ptr<MeshBatch>> &rMeshBatches) : MeshBatchingStrategy(rMeshBatches)
		{
		}

		void AddMeshFilter(MeshFilter *pMeshFilter) override;
		void RemoveMeshFilter(MeshFilter *pMeshFilter) override;
	};

}

#endif