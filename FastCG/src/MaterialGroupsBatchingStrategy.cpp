#include <FastCG/MaterialGroupsBatchingStrategy.h>
#include <FastCG/Material.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	void MaterialGroupsBatchingStrategy::AddMeshFilter(MeshFilter* pMeshFilter)
	{
		auto pMaterial = pMeshFilter->GetMaterial();
		if (pMaterial == nullptr)
		{
			return;
		}

		bool added = false;

		for (const auto& pRenderBatch : mrRenderBatches)
		{
			if (pRenderBatch->pMaterial == pMaterial)
			{
				auto& rMeshFilters = pRenderBatch->meshFilters;
				auto it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
				assert(it == rMeshFilters.end());
				rMeshFilters.emplace_back(pMeshFilter);
				added = true;
				break;
			}
		}

		if (added)
		{
			return;
		}

		auto* pNewRenderBatch = new RenderBatch();
		pNewRenderBatch->pMaterial = pMaterial;
		pNewRenderBatch->meshFilters.emplace_back(pMeshFilter);
		mrRenderBatches.emplace_back(pNewRenderBatch);
	}

	void MaterialGroupsBatchingStrategy::RemoveMeshFilter(MeshFilter* pMeshFilter)
	{
		auto pMaterial = pMeshFilter->GetMaterial();
		if (pMaterial == nullptr)
		{
			return;
		}

		auto it = mrRenderBatches.begin();
		bool removed = false;

		while (it != mrRenderBatches.end())
		{
			if ((*it)->pMaterial == pMaterial)
			{
				auto& rMeshFilters = (*it)->meshFilters;
				auto it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
				assert(it != rMeshFilters.end());
				rMeshFilters.erase(it);
				removed = true;
				break;
			}
		}

		assert(removed);

		if ((*it)->meshFilters.size() == 0)
		{
			mrRenderBatches.erase(it);
		}
	}

}