#include <FastCG/MeshFilter.h>
#include <FastCG/BaseApplication.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(MeshFilter, Component);

	void MeshFilter::SetMaterial(const std::shared_ptr<Material> &pMaterial)
	{
		BaseApplication::GetInstance()->BeforeMeshFilterChange(this);
		mpMaterial = pMaterial;
		BaseApplication::GetInstance()->AfterMeshFilterChange(this);
	}

}