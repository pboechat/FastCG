#include <UnorderedSingleGroupBatchingStrategy.h>

#include <algorithm>

void UnorderedSingleGroupBatchingStrategy::AddMeshFilter(MeshFilter* pMeshFilter)
{
	RenderBatch* pRenderBatch;
	if (mrRenderBatches.size() == 0)
	{
		pRenderBatch = new RenderBatch();
		mrRenderBatches.push_back(pRenderBatch);
	}
	else
	{
		pRenderBatch = mrRenderBatches[0];
	}

	std::vector<MeshFilter*>& rMeshFilters = pRenderBatch->meshFilters;
	std::vector<MeshFilter*>::iterator it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);

	if (it != rMeshFilters.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "it != rMeshFilters.end()");
	}

	pRenderBatch->meshFilters.push_back(pMeshFilter);
}

void UnorderedSingleGroupBatchingStrategy::RemoveMeshFilter(MeshFilter* pMeshFilter)
{
	if (mrRenderBatches.size() == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mrRenderBatches.size() == 0");
	}

	RenderBatch* pRenderBatch = mrRenderBatches[0];
	std::vector<MeshFilter*>& rMeshFilters = pRenderBatch->meshFilters;
	std::vector<MeshFilter*>::iterator it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);

	if (it == rMeshFilters.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "it == rMeshFilters.end()");
	}

	rMeshFilters.erase(it);
}
