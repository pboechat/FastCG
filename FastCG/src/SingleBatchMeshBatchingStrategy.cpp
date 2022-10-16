#include <FastCG/SingleBatchMeshBatchingStrategy.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	void SingleBatchMeshBatchingStrategy::AddMeshFilter(MeshFilter *pMeshFilter)
	{
		MeshBatch *pMeshBatch;
		if (mrMeshBatches.size() == 0)
		{
			pMeshBatch = new MeshBatch();
			mrMeshBatches.emplace_back(pMeshBatch);
		}
		else
		{
			pMeshBatch = mrMeshBatches[0].get();
		}

		auto &rMeshFilters = pMeshBatch->meshFilters;
		auto it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
		assert(it == rMeshFilters.end());

		pMeshBatch->meshFilters.emplace_back(pMeshFilter);
	}

	void SingleBatchMeshBatchingStrategy::RemoveMeshFilter(MeshFilter *pMeshFilter)
	{
		assert(mrMeshBatches.size() != 0);
		auto *pMeshBatch = mrMeshBatches[0].get();
		auto &rMeshFilters = pMeshBatch->meshFilters;
		auto it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
		assert(it != rMeshFilters.end());
		rMeshFilters.erase(it);
	}

}
