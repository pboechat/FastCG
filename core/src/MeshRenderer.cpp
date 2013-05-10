#include <MeshRenderer.h>

IMPLEMENT_TYPE(MeshRenderer, Renderer);

void MeshRenderer::OnRender()
{
	if (mMeshPtr == 0)
	{
		return;
	}

	mMeshPtr->DrawCall();
}
