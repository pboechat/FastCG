#include <FirstPersonCameraController.h>
#include <Application.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <Camera.h>

IMPLEMENT_TYPE(FirstPersonCameraController, Behaviour);

void FirstPersonCameraController::OnUpdate(float time, float deltaTime)
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
				mainCameraPtr->GetGameObject()->GetTransform()->Rotate(direction.x * mTurnSpeed * deltaTime, glm::vec3(0.0f, -1.0f, 0.0f));
				mainCameraPtr->GetGameObject()->GetTransform()->Rotate(direction.y * mTurnSpeed * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
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
		mainCameraPtr->GetGameObject()->GetTransform()->Translate(glm::vec3(-1.0f, 0.0f, 0.0f) * mWalkSpeed * deltaTime);
	}

	else if (Input::GetKey(KeyCode::RIGHT_ARROW) || Input::GetKey(KeyCode::LETTER_D))
	{
		mainCameraPtr->GetGameObject()->GetTransform()->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * mWalkSpeed * deltaTime);
	}

	if (Input::GetKey(KeyCode::UP_ARROW) || Input::GetKey(KeyCode::LETTER_W))
	{
		mainCameraPtr->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 0.0f, -1.0f) * mWalkSpeed * deltaTime);
	}

	else if (Input::GetKey(KeyCode::DOWN_ARROW) || Input::GetKey(KeyCode::LETTER_S))
	{
		mainCameraPtr->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 0.0f, 1.0f) * mWalkSpeed * deltaTime);
	}
}

