#include <FastCG/MaterialBasedMeshBatchingStrategy.h>
#include <FastCG/Material.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	void MaterialBasedMeshBatchingStrategy::AddMeshFilter(MeshFilter *pMeshFilter)
	{
		auto pMaterial = pMeshFilter->GetMaterial();
		if (pMaterial == nullptr)
		{
			return;
		}

		bool added = false;

		for (const auto &pMeshBatch : mrMeshBatches)
		{
			if (pMeshBatch->pMaterial == pMaterial)
			{
				auto &rMeshFilters = pMeshBatch->meshFilters;
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

		auto pNewMeshBatch = std::make_unique<MeshBatch>();
		pNewMeshBatch->pMaterial = pMaterial;
		pNewMeshBatch->meshFilters.emplace_back(pMeshFilter);
		mrMeshBatches.emplace_back(std::move(pNewMeshBatch));
	}

	void MaterialBasedMeshBatchingStrategy::RemoveMeshFilter(MeshFilter *pMeshFilter)
	{
		auto pMaterial = pMeshFilter->GetMaterial();
		if (pMaterial == nullptr)
		{
			return;
		}

		auto it = mrMeshBatches.begin();
		bool removed = false;

		while (it != mrMeshBatches.end())
		{
			if ((*it)->pMaterial == pMaterial)
			{
				auto &rMeshFilters = (*it)->meshFilters;
				auto it2 = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);
				assert(it2 != rMeshFilters.end());
				rMeshFilters.erase(it2);
				removed = true;
				break;
			}
			it++;
		}

		assert(removed);

		if ((*it)->meshFilters.size() == 0)
		{
			mrMeshBatches.erase(it);
		}
	}

}