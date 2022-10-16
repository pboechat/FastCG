#ifndef FASTCG_MESH_BATCHING_STRATEGY_H
#define FASTCG_MESH_BATCHING_STRATEGY_H

#include <FastCG/MeshBatch.h>
#include <FastCG/MeshFilter.h>

#include <vector>
#include <memory>

namespace FastCG
{
	class MeshBatchingStrategy
	{
	public:
		MeshBatchingStrategy(std::vector<std::unique_ptr<MeshBatch>> &rMeshBatches) : mrMeshBatches(rMeshBatches)
		{
		}

		virtual void AddMeshFilter(MeshFilter *pMeshFilter) = 0;
		virtual void RemoveMeshFilter(MeshFilter *pMeshFilter) = 0;

	protected:
		std::vector<std::unique_ptr<MeshBatch>> &mrMeshBatches;
	};

}

#endif