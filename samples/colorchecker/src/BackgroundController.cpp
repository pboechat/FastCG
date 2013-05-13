#include "BackgroundController.h"

#include <Input.h>
#include <MouseButton.h>
#include <MathT.h>

#include <iostream>

COMPONENT_IMPLEMENTATION(BackgroundController, Behaviour);

void BackgroundController::OnUpdate(float time, float deltaTime)
{
	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftMouseButtonClickTime > 0.333f)
	{
		const glm::vec2& mousePosition = Input::GetMousePosition();

		sRGBColor color = mBaseColors[GetColorPatchIndex((int)mousePosition.x, (int)mousePosition.y)];
		std::cout << "Patch: (" << color.R() << ", " << color.G() << ", " << color.B() << ")" << std::endl;

		mLastLeftMouseButtonClickTime = time;
	}

	glm::vec4 clearColor = Application::GetInstance()->GetClearColor();

	// zoom in (lighten)
	if (Input::GetMouseWheelDelta() > 0)
	{
		clearColor += glm::vec4(0.01f, 0.01f, 0.01f, 0.0f);
	}
	// zoom out (darken)
	else if (Input::GetMouseWheelDelta() < 0)
	{
		clearColor -= glm::vec4(0.01f, 0.01f, 0.01f, 0.0f);
	}

	Application::GetInstance()->SetClearColor(glm::clamp(clearColor, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
}

int BackgroundController::GetColorPatchIndex(int x, int y)
{
	float patchWidth = Application::GetInstance()->GetScreenWidth() / (float) mHorizontalNumberOfColorPatches;
	float patchHeight = Application::GetInstance()->GetScreenHeight() / (float) mVerticalNumberOfColorPatches;

	int patchX = (int)MathF::Floor(x / patchWidth);
	int patchY = (int)MathF::Floor(y / patchHeight);

	return patchY * mHorizontalNumberOfColorPatches + patchX;
}
