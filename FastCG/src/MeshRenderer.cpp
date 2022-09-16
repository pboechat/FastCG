#include <FastCG/MeshRenderer.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(MeshRenderer, Renderer);

	void MeshRenderer::OnRender()
	{
		for (auto& pMesh : mMeshes)
		{
			pMesh->Draw();
		}
	}

}