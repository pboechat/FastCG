#include <MeshFilter.h>
#include <Application.h>

COMPONENT_IMPLEMENTATION(MeshFilter, Component);

void MeshFilter::SetMaterial(Material* pMaterial)
{
	if (mpMaterial != 0)
	{
		Application::GetInstance()->RemoveFromMeshRenderingGroup(this, mpMaterial);
	}

	mpMaterial = pMaterial;

	if (mpMaterial != 0)
	{
		Application::GetInstance()->AddToMeshRenderingGroup(this, mpMaterial);
	}
}