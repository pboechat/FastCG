#include <FastCG/MouseButton.h>
#include <FastCG/MouseButton.h>
#include <FastCG/MathT.h>
#include <FastCG/KeyCode.h>
#include <FastCG/Input.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/Camera.h>
#include <FastCG/Application.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(FlyCameraController, Behaviour);

	constexpr float EPSILON = 1;

	void FlyCameraController::OnUpdate(float time, float deltaTime)
	{
		auto *mainCamera = Application::GetInstance()->GetMainCamera();
		if (mainCamera == nullptr)
		{
			return;
		}

		auto *pTransform = mainCamera->GetGameObject()->GetTransform();

		if (Input::GetMouseButton(MouseButton::RIGHT_BUTTON) == MouseButtonState::PRESSED)
		{
			if (mRightMouseButtonPressed)
			{
				auto mousePosition = Input::GetMousePosition();

				if (mousePosition != mLastMousePosition)
				{
					auto direction = mousePosition - mLastMousePosition;

					auto axis = glm::vec3(MathF::Abs(direction.y) > EPSILON ? direction.y : 0, MathF::Abs(direction.x) > EPSILON ? -direction.x : 0, 0);
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

		if (Input::GetKey(KeyCode::PAGE_UP) || Input::GetKey(KeyCode::LETTER_Q))
		{
			pTransform->SetRotation(pTransform->GetRotation() * glm::angleAxis(mTurnSpeed * deltaTime, glm::vec3(0, 0, 1)));
		}
		else if (Input::GetKey(KeyCode::PAGE_DOWN) || Input::GetKey(KeyCode::LETTER_E))
		{
			pTransform->SetRotation(pTransform->GetRotation() * glm::angleAxis(-mTurnSpeed * deltaTime, glm::vec3(0, 0, 1)));
		}

		auto position = pTransform->GetPosition();
		if (Input::GetKey(KeyCode::LEFT_ARROW) || Input::GetKey(KeyCode::LETTER_A))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(-1, 0, 0) * mWalkSpeed * deltaTime));
		}
		else if (Input::GetKey(KeyCode::RIGHT_ARROW) || Input::GetKey(KeyCode::LETTER_D))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(1, 0, 0) * mWalkSpeed * deltaTime));
		}

		if (Input::GetKey(KeyCode::UP_ARROW) || Input::GetKey(KeyCode::LETTER_W))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(0, 0, -1) * mWalkSpeed * deltaTime));
		}
		else if (Input::GetKey(KeyCode::DOWN_ARROW) || Input::GetKey(KeyCode::LETTER_S))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(0, 0, 1) * mWalkSpeed * deltaTime));
		}
	}

}
