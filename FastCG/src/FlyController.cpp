#include <FastCG/Transform.h>
#include <FastCG/MouseButton.h>
#include <FastCG/MathT.h>
#include <FastCG/Key.h>
#include <FastCG/InputSystem.h>
#include <FastCG/FlyController.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(FlyController, Behaviour);

	void FlyController::OnUpdate(float time, float deltaTime)
	{
		auto *pTransform = GetGameObject()->GetTransform();

		auto &rotation = pTransform->GetRotation();

		if (InputSystem::GetMouseButton(MouseButton::RIGHT_BUTTON) == MouseButtonState::PRESSED)
		{
			if (mRightMouseButtonPressed)
			{
				auto mousePosition = InputSystem::GetMousePosition();

				if (mousePosition != mLastMousePosition)
				{
					if (rotation != mRotation)
					{
						const auto halfDegToRad = 0.5f * MathF::DEGREES_TO_RADIANS;

						auto euler = glm::eulerAngles(rotation);

						// FIXME: can't map non-multiple of PI rotations around the Z axis for now
						assert(fmod(euler.z, 180) == 0);

						mTheta = euler.x;
						if (euler.z == 180)
						{
							mTheta -= 180;
						}
						mTheta *= halfDegToRad;
						if (mTheta < 0)
						{
							mTheta = MathF::TWO_PI + mTheta;
						}

						mPhi = euler.y;
						if (euler.z == 180)
						{
							mPhi += 180;
						}
						mPhi *= halfDegToRad;
						if (mPhi < 0)
						{
							mPhi = MathF::TWO_PI + mPhi;
						}
					}

					auto delta = deltaTime * mTurnSpeed;

					auto thetaDelta = (mousePosition.y - (float)mLastMousePosition.y) * delta;
					auto phiDelta = (mousePosition.x - (float)mLastMousePosition.x) * delta;

					mTheta = fmod(mTheta + thetaDelta, MathF::TWO_PI);
					mPhi = fmod(mPhi + phiDelta, MathF::TWO_PI);

					// Based on: https://github.com/moble/quaternionic/blob/main/quaternionic/converters.py

					float cosTheta = cos(mTheta);
					float sinTheta = sin(mTheta);
					float cosPhi = cos(mPhi);
					float sinPhi = sin(mPhi);

					mRotation = glm::quat{-cosTheta * cosPhi, sinTheta * cosPhi, cosTheta * sinPhi, sinTheta * sinPhi};

					pTransform->SetRotation(mRotation);

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

		auto position = pTransform->GetPosition();

		if (InputSystem::GetKey(Key::PAGE_UP) || InputSystem::GetKey(Key::LETTER_Q))
		{
			pTransform->SetPosition(position + (rotation * glm::vec3(0, -1, 0) * mWalkSpeed * deltaTime));
		}
		else if (InputSystem::GetKey(Key::PAGE_DOWN) || InputSystem::GetKey(Key::LETTER_E))
		{
			pTransform->SetPosition(position + (rotation * glm::vec3(0, 1, 0) * mWalkSpeed * deltaTime));
		}

		if (InputSystem::GetKey(Key::LEFT_ARROW) || InputSystem::GetKey(Key::LETTER_A))
		{
			pTransform->SetPosition(position + (rotation * glm::vec3(-1, 0, 0) * mWalkSpeed * deltaTime));
		}
		else if (InputSystem::GetKey(Key::RIGHT_ARROW) || InputSystem::GetKey(Key::LETTER_D))
		{
			pTransform->SetPosition(position + (rotation * glm::vec3(1, 0, 0) * mWalkSpeed * deltaTime));
		}

		if (InputSystem::GetKey(Key::UP_ARROW) || InputSystem::GetKey(Key::LETTER_W))
		{
			pTransform->SetPosition(position + (rotation * glm::vec3(0, 0, -1) * mWalkSpeed * deltaTime));
		}
		else if (InputSystem::GetKey(Key::DOWN_ARROW) || InputSystem::GetKey(Key::LETTER_S))
		{
			pTransform->SetPosition(position + (rotation * glm::vec3(0, 0, 1) * mWalkSpeed * deltaTime));
		}
	}

}
