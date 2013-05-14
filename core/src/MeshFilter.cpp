#include <MeshFilter.h>
#include <Application.h>

COMPONENT_IMPLEMENTATION(MeshFilter, Component);

void MeshFilter::SetMaterial(Material* pMaterial)
{
	Application::GetInstance()->BeforeMeshFilterChange(this);
	mpMaterial = pMaterial;
	Application::GetInstance()->AfterMeshFilterChange(this);
}