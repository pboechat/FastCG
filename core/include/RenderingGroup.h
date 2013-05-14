#ifndef RENDERINGGROUP_H_
#define RENDERINGGROUP_H_

#include <Material.h>
#include <MeshFilter.h>

#include <vector>

struct RenderingGroup
{
	Material* pMaterial;
	std::vector<MeshFilter*> meshFilters;
};

#endif