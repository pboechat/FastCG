#ifndef FASTCG_RENDER_BATCH_H_
#define FASTCG_RENDER_BATCH_H_

#include <FastCG/Material.h>
#include <FastCG/MeshFilter.h>

#include <vector>
#include <memory>

namespace FastCG
{
	struct RenderBatch
	{
		std::shared_ptr<Material> pMaterial;
		std::vector<MeshFilter*> meshFilters;

	};

}

#endif