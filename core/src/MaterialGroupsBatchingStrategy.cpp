#include <MaterialGroupsBatchingStrategy.h>
#include <Material.h>

#include <algorithm>

void MaterialGroupsBatchingStrategy::AddMeshFilter(MeshFilter* pMeshFilter)
{
	Material* pMaterial = pMeshFilter->GetMaterial();

	if (pMaterial == 0)
	{
		return;
	}

	bool added = false;

	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderBatch = mrRenderBatches[i];

		if (pRenderBatch->pMaterial == pMaterial)
		{
			pRenderBatch->meshFilters.push_back(pMeshFilter);
			added = true;
			break;
		}
	}

	if (added)
	{
		return;
	}

	RenderBatch* pNewRenderBatch = new RenderBatch();
	pNewRenderBatch->pMaterial = pMaterial;
	pNewRenderBatch->meshFilters.push_back(pMeshFilter);
	mrRenderBatches.push_back(pNewRenderBatch);
}

void MaterialGroupsBatchingStrategy::RemoveMeshFilter(MeshFilter* pMeshFilter)
{
	Material* pMaterial = pMeshFilter->GetMaterial();

	if (pMaterial == 0)
	{
		return;
	}

	RenderBatch* pRenderBatch;
	bool removed = false;

	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		pRenderBatch = mrRenderBatches[i];

		if (pRenderBatch->pMaterial == pMaterial)
		{
			std::vector<MeshFilter*>& rMeshFilters = pRenderBatch->meshFilters;
			std::vector<MeshFilter*>::iterator it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);

			if (it == rMeshFilters.end())
			{
				// FIXME: checking invariants
				THROW_EXCEPTION(Exception, "it == rMeshFilters.end()");
			}

			rMeshFilters.erase(it);
			removed = true;
			break;
		}
	}

	if (!removed)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "!removed");
	}

	if (pRenderBatch->meshFilters.size() == 0)
	{
		std::vector<RenderBatch*>::iterator it = std::find(mrRenderBatches.begin(), mrRenderBatches.end(), pRenderBatch);
		mrRenderBatches.erase(it);
		delete pRenderBatch;
	}
}