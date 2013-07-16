#include <FirstPersonCameraController.h>
#include <Application.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <Camera.h>
#include <MathT.h>

COMPONENT_IMPLEMENTATION(FirstPersonCameraController, Behaviour);

const float FirstPersonCameraController::PITCH_LIMIT = 60.0f;

void FirstPersonCameraController::OnUpdate(float time, float deltaTime)
{
	Transform* pTransform = Application::GetInstance()->GetMainCamera()->GetGameObject()->GetTransform();

	if (Input::GetMouseButton(MouseButton::RIGHT_BUTTON))
	{
		if (mRightMouseButtonPressed)
		{
			glm::vec2 mousePosition = Input::GetMousePosition();

			if (mousePosition != mLastMousePosition)
			{
				glm::vec2 direction = glm::normalize(mousePosition - mLastMousePosition);
				glm::vec3 axis(direction.y, -direction.x, 0);
				pTransform->SetRotation(pTransform->GetRotation() * glm::angleAxis(mTurnSpeed * deltaTime, axis));
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

	glm::vec3 position = pTransform->GetPosition();
	glm::vec3 movement;
	if (Input::GetKey(KeyCode::LEFT_ARROW) || Input::GetKey(KeyCode::LETTER_A))
	{
		movement = glm::vec3(-1.0f, 0.0f, 0.0f) * mWalkSpeed * deltaTime;
		if (mFlying)
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * movement));
		}
		else
		{
			pTransform->SetPosition(position + movement);
		}
	}

	else if (Input::GetKey(KeyCode::RIGHT_ARROW) || Input::GetKey(KeyCode::LETTER_D))
	{
		movement = glm::vec3(1.0f, 0.0f, 0.0f) * mWalkSpeed * deltaTime;
		if (mFlying)
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * movement));
		}
		else 
		{
			pTransform->SetPosition(position + movement);
		}
	}

	if (Input::GetKey(KeyCode::UP_ARROW) || Input::GetKey(KeyCode::LETTER_W))
	{
		movement = glm::vec3(0.0f, 0.0f, -1.0f) * mWalkSpeed * deltaTime;
		if (mFlying)
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * movement));
		}
		else
		{
			pTransform->SetPosition(position + movement);
		}
	}

	else if (Input::GetKey(KeyCode::DOWN_ARROW) || Input::GetKey(KeyCode::LETTER_S))
	{
		movement = glm::vec3(0.0f, 0.0f, 1.0f) * mWalkSpeed * deltaTime;
		if (mFlying)
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * movement));
		}
		else
		{
			pTransform->SetPosition(position + movement);
		}
	}

	else if (Input::GetKey(KeyCode::PAGE_UP) || Input::GetKey(KeyCode::LETTER_Q))
	{
		pTransform->SetPosition(position + glm::vec3(0.0f, 1.0f, 0.0f) * mWalkSpeed * deltaTime);
	}

	else if (Input::GetKey(KeyCode::PAGE_DOWN) || Input::GetKey(KeyCode::LETTER_E))
	{
		pTransform->SetPosition(position + glm::vec3(0.0f, -1.0f, 0.0f) * mWalkSpeed * deltaTime);
	}
}

