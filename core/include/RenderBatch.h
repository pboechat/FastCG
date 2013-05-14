#ifndef RENDERBATCH_H_
#define RENDERBATCH_H_

#include <Material.h>
#include <MeshFilter.h>

#include <vector>

struct RenderBatch
{
	Material* pMaterial;
	std::vector<MeshFilter*> meshFilters;
};

#endif