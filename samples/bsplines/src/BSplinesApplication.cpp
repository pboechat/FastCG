#include "BSplinesApplication.h"
#include <Camera.h>

BSplinesApplication::BSplinesApplication() :
	Application("bsplines", 800, 600)
{
	mMainCameraPtr = new Camera(0.0f, 1.0f, -1.0f, 0.0f, (float) GetScreenHeight(), 0.0f, (float) GetScreenWidth(), PM_ORTHOGRAPHIC);
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

BSplinesApplication::~BSplinesApplication()
{
}

void BSplinesApplication::OnStart()
{
}

void BSplinesApplication::OnMouseButton(int button, int state, int x, int y)
{
}