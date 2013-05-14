#include "DeferredShadingApplication.h"

#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>

DeferredShadingApplication::DeferredShadingApplication() :
	Application("deferredshading", 1024, 768),
	mpMesh(0)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mDeferredRendering = true;
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void DeferredShadingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 0.0f, 5.0f));

	GameObject* pGameObject = GameObject::Instantiate();

	mpMesh = StandardGeometries::CreateSphere(1.0f, 10);

	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);
	pMeshRenderer->SetMesh(mpMesh);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGameObject);
}

void DeferredShadingApplication::OnEnd()
{
	if (mpMesh != 0)
	{
		delete mpMesh;
	}
}
