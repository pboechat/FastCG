#include <FirstPersonCameraController.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <Application.h>
#include <Camera.h>

void FirstPersonCameraController::Update(float time, float deltaTime)
{
	CameraPtr mainCameraPtr = Application::GetInstance()->GetMainCamera();

	if (Input::GetMouseButton(MouseButton::RIGHT_BUTTON))
	{
		if (mRightMouseButtonPressed)
		{
			glm::vec2 mousePosition = Input::GetMousePosition();

			if (mousePosition != mLastMousePosition)
			{
				glm::vec2 direction = glm::normalize(mousePosition - mLastMousePosition);
				mainCameraPtr->Rotate(direction.x * mTurnSpeed * deltaTime, glm::vec3(0.0f, -1.0f, 0.0f));
				mainCameraPtr->Rotate(direction.y * mTurnSpeed * deltaTime, glm::vec3(-1.0f, 0.0f, 0.0f));
				mLastMousePosition = mousePosition;
			}
		}

		else
		{
			mRightMouseButtonPressed = true;
			mLastMousePosition = Input::GetMousePosition();
		}
	}

	else
	{
		mRightMouseButtonPressed = false;
	}

	if (Input::GetKey(KeyCode::LEFT_ARROW) || Input::GetKey(KeyCode::LETTER_A))
	{
		mainCameraPtr->Translate(glm::vec3(-1.0f, 0.0f, 0.0f) * mWalkSpeed * deltaTime);
	}

	else if (Input::GetKey(KeyCode::RIGHT_ARROW) || Input::GetKey(KeyCode::LETTER_D))
	{
		mainCameraPtr->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * mWalkSpeed * deltaTime);
	}

	if (Input::GetKey(KeyCode::UP_ARROW) || Input::GetKey(KeyCode::LETTER_W))
	{
		mainCameraPtr->Translate(glm::vec3(0.0f, 0.0f, -1.0f) * mWalkSpeed * deltaTime);
	}

	else if (Input::GetKey(KeyCode::DOWN_ARROW) || Input::GetKey(KeyCode::LETTER_S))
	{
		mainCameraPtr->Translate(glm::vec3(0.0f, 0.0f, 1.0f) * mWalkSpeed * deltaTime);
	}
}

