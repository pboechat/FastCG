#ifndef FASTCG_MESH_BATCH_H
#define FASTCG_MESH_BATCH_H

#include <FastCG/Material.h>
#include <FastCG/MeshFilter.h>

#include <vector>
#include <memory>

namespace FastCG
{
	struct MeshBatch
	{
		std::shared_ptr<Material> pMaterial;
		std::vector<MeshFilter *> meshFilters;
	};

}

#endif