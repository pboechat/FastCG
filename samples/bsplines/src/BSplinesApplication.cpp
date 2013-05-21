#include "BSplinesApplication.h"
#include "BSplinesEditor.h"

#include <LineRenderer.h>
#include <PointsRenderer.h>

BSplinesApplication::BSplinesApplication() :
	Application("bsplines", 800, 600, 30)
{
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mShowFPS = true;
}

void BSplinesApplication::OnStart()
{
	mpMainCamera->SetUp(0.0f, 1.0f, -1.0f, 0.0f, (float) GetScreenHeight(), 0.0f, (float) GetScreenWidth(), PM_ORTHOGRAPHIC);

	GameObject* pBSplineGameObject = GameObject::Instantiate();
	LineRenderer::Instantiate(pBSplineGameObject);

	GameObject* pControlPolygonGameObject = GameObject::Instantiate();
	LineRenderer::Instantiate(pControlPolygonGameObject);

	GameObject* pControlPointsGameObject = GameObject::Instantiate();
	PointsRenderer::Instantiate(pControlPointsGameObject);

	GameObject* pKnotsGameObject = GameObject::Instantiate();
	PointsRenderer::Instantiate(pKnotsGameObject);

	GameObject* pBSplinesEditorGameObject = GameObject::Instantiate();

	BSplinesEditor* bSplinesEditor = BSplinesEditor::Instantiate(pBSplinesEditorGameObject);
	bSplinesEditor->SetBSplineGameObject(pBSplineGameObject);
	bSplinesEditor->SetControlPolygonGameObject(pControlPolygonGameObject);
	bSplinesEditor->SetControlPointsGameObject(pControlPointsGameObject);
	bSplinesEditor->SetKnotsGameObject(pKnotsGameObject);
}
