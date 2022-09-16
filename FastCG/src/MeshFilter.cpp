#include <FastCG/MeshFilter.h>
#include <FastCG/Application.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(MeshFilter, Component);

	void MeshFilter::SetMaterial(const std::shared_ptr<Material>& pMaterial)
	{
		Application::GetInstance()->BeforeMeshFilterChange(this);
		mpMaterial = pMaterial;
		Application::GetInstance()->AfterMeshFilterChange(this);
	}

}