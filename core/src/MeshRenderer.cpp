#include <MeshRenderer.h>
#include <Exception.h>

COMPONENT_IMPLEMENTATION(MeshRenderer, Renderer);

void MeshRenderer::OnRender()
{
	for (unsigned int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i]->DrawCall();
	}
}

void MeshRenderer::RemoveMesh(Mesh* pMesh)
{
	std::vector<Mesh*>::iterator it = std::find(mMeshes.begin(), mMeshes.end(), pMesh);

	if (it == mMeshes.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "it == mMeshes.end()");
	}

	mMeshes.erase(it);
}

unsigned int MeshRenderer::GetNumberOfTriangles() const
{
	unsigned int totalNumberOfTriangles = 0;
	for (unsigned int i = 0; i < mMeshes.size(); i++) 
	{
		totalNumberOfTriangles += mMeshes[i]->GetNumberOfTriangles();
	}
	return totalNumberOfTriangles;
}
