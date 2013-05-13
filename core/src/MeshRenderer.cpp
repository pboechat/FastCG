#include <MeshRenderer.h>

COMPONENT_IMPLEMENTATION(MeshRenderer, Renderer);

void MeshRenderer::OnRender()
{
	if (mpMesh == 0)
	{
		return;
	}

	mpMesh->DrawCall();
}
