#include <FastCG/MouseButton.h>
#include <FastCG/MouseButton.h>
#include <FastCG/MathT.h>
#include <FastCG/KeyCode.h>
#include <FastCG/InputSystem.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/Camera.h>
#include <FastCG/BaseApplication.h>

namespace
{
	constexpr float EPSILON = 1;

}

namespace FastCG
{
	IMPLEMENT_COMPONENT(FlyCameraController, Behaviour);

	void FlyCameraController::OnUpdate(float time, float deltaTime)
	{
		auto *mainCamera = BaseApplication::GetInstance()->GetMainCamera();
		if (mainCamera == nullptr)
		{
			return;
		}

		auto *pTransform = mainCamera->GetGameObject()->GetTransform();

		if (InputSystem::GetMouseButton(MouseButton::RIGHT_BUTTON) == MouseButtonState::PRESSED)
		{
			if (mRightMouseButtonPressed)
			{
				auto mousePosition = InputSystem::GetMousePosition();

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
				mLastMousePosition = InputSystem::GetMousePosition();
			}
		}
		else
		{
			mRightMouseButtonPressed = false;
		}

		if (InputSystem::GetKey(KeyCodes::PAGE_UP) || InputSystem::GetKey(KeyCodes::LETTER_Q))
		{
			pTransform->SetRotation(pTransform->GetRotation() * glm::angleAxis(mTurnSpeed * deltaTime, glm::vec3(0, 0, 1)));
		}
		else if (InputSystem::GetKey(KeyCodes::PAGE_DOWN) || InputSystem::GetKey(KeyCodes::LETTER_E))
		{
			pTransform->SetRotation(pTransform->GetRotation() * glm::angleAxis(-mTurnSpeed * deltaTime, glm::vec3(0, 0, 1)));
		}

		auto position = pTransform->GetPosition();
		if (InputSystem::GetKey(KeyCodes::LEFT_ARROW) || InputSystem::GetKey(KeyCodes::LETTER_A))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(-1, 0, 0) * mWalkSpeed * deltaTime));
		}
		else if (InputSystem::GetKey(KeyCodes::RIGHT_ARROW) || InputSystem::GetKey(KeyCodes::LETTER_D))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(1, 0, 0) * mWalkSpeed * deltaTime));
		}

		if (InputSystem::GetKey(KeyCodes::UP_ARROW) || InputSystem::GetKey(KeyCodes::LETTER_W))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(0, 0, -1) * mWalkSpeed * deltaTime));
		}
		else if (InputSystem::GetKey(KeyCodes::DOWN_ARROW) || InputSystem::GetKey(KeyCodes::LETTER_S))
		{
			pTransform->SetPosition(position + (pTransform->GetRotation() * glm::vec3(0, 0, 1) * mWalkSpeed * deltaTime));
		}
	}

}
