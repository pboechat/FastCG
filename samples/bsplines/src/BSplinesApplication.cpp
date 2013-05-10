#include "BSplinesApplication.h"
#include "BSplinesEditor.h"

#include <LineRenderer.h>
#include <PointsRenderer.h>

BSplinesApplication::BSplinesApplication() :
	Application("bsplines", 800, 600)
{
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	mShowFPS = true;
}

BSplinesApplication::~BSplinesApplication()
{
}

void BSplinesApplication::OnStart()
{
	mpMainCamera->SetUp(0.0f, 1.0f, -1.0f, 0.0f, (float) GetScreenHeight(), 0.0f, (float) GetScreenWidth(), PM_ORTHOGRAPHIC);

	GameObjectPtr bSplineGameObjectPtr = new GameObject();

	LineRendererPtr lineRendererPtr = new LineRenderer();
	bSplineGameObjectPtr->AddComponent(lineRendererPtr);

	GameObjectPtr controlPolygonGameObjectPtr = new GameObject();

	lineRendererPtr = new LineRenderer();
	controlPolygonGameObjectPtr->AddComponent(lineRendererPtr);

	GameObjectPtr controlPointsGameObjectPtr = new GameObject();

	PointsRendererPtr pointsRendererPtr = new PointsRenderer();
	controlPointsGameObjectPtr->AddComponent(pointsRendererPtr);

	GameObjectPtr knotsGameObjectPtr = new GameObject();

	pointsRendererPtr = new PointsRenderer();
	knotsGameObjectPtr->AddComponent(pointsRendererPtr);

	GameObjectPtr bSplinesEditorGameObjectPtr = new GameObject();

	BSplinesEditor* bSplinesEditor = new BSplinesEditor();
	bSplinesEditor->SetBSplineGameObject(bSplineGameObjectPtr);
	bSplinesEditor->SetControlPolygonGameObject(controlPolygonGameObjectPtr);
	bSplinesEditor->SetControlPointsGameObject(controlPointsGameObjectPtr);
	bSplinesEditor->SetKnotsGameObject(knotsGameObjectPtr);

	bSplinesEditorGameObjectPtr->AddComponent(bSplinesEditor);
}
