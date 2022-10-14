#include <FastCG/UnorderedSingleGroupBatchingStrategy.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	void UnorderedSingleGroupBatchingStrategy::AddMeshFilter(MeshFilter *pMeshFilter)
	{
		RenderBatch *pRenderBatch;
		if (mrRenderBatches.size() == 0)
		{
			pRenderBatch = new RenderBatch();
			mrRenderBatches.emplace_back(pRenderBatch);
		}
		else
		{
			pRenderBatch = mrRenderBatches[0].get();
		}

		auto &rMeshFilters = pRenderBatch->meshFilters;
		auto it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
		assert(it == rMeshFilters.end());

		pRenderBatch->meshFilters.emplace_back(pMeshFilter);
	}

	void UnorderedSingleGroupBatchingStrategy::RemoveMeshFilter(MeshFilter *pMeshFilter)
	{
		assert(mrRenderBatches.size() != 0);
		auto *pRenderBatch = mrRenderBatches[0].get();
		auto &rMeshFilters = pRenderBatch->meshFilters;
		auto it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
		assert(it != rMeshFilters.end());
		rMeshFilters.erase(it);
	}

}
