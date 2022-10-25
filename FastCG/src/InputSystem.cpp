#include <FastCG/MouseButton.h>
#include <FastCG/MathT.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Exception.h>

#include <cstring>

namespace FastCG
{
	bool InputSystem::GetKey(Key keyCode)
	{
		return GetInstance()->mFrontBuffer.GetKey(keyCode);
	}

	MouseButtonState InputSystem::GetMouseButton(MouseButton button)
	{
		return GetInstance()->mFrontBuffer.GetMouseButton(button);
	}

	const glm::uvec2 &InputSystem::GetMousePosition()
	{
		return GetInstance()->mFrontBuffer.GetMousePosition();
	}

	int InputSystem::GetMouseWheelDelta()
	{
		return GetInstance()->mFrontBuffer.GetMouseWheelDelta();
	}

	void InputSystem::SetKey(Key keyCode, bool pressed)
	{
		mBackBuffer.SetKey(keyCode, pressed);
	}

	void InputSystem::SetMouseButton(MouseButton button, MouseButtonState state)
	{
		mBackBuffer.SetMouseButton(button, state);
	}

	void InputSystem::SetMousePosition(const glm::uvec2 &rPosition)
	{
		mBackBuffer.SetMousePosition(rPosition);
	}

	void InputSystem::IncrementMouseWheelDelta()
	{
		mBackBuffer.IncrementMouseWheelDelta();
	}

	void InputSystem::DecrementMouseWheelDelta()
	{
		mBackBuffer.DecrementMouseWheelDelta();
	}

	void InputSystem::Swap()
	{
		mFrontBuffer.Copy(mBackBuffer);
		mBackBuffer.Clear();
	}

	InputSystem::InputBuffer::InputBuffer()
	{
		memset(mpKeys, 0, sizeof(mpKeys));
	}

	bool InputSystem::InputBuffer::GetKey(Key keyCode) const
	{
		return mpKeys[(KeyCodesInt)keyCode];
	}

	MouseButtonState InputSystem::InputBuffer::GetMouseButton(MouseButton button) const
	{
		if (button == MouseButton::LEFT_BUTTON)
		{
			return mLeftMouseButton;
		}
		else if (button == MouseButton::MIDDLE_BUTTON)
		{
			return mMiddleMouseButton;
		}
		else
		{
			return mRightMouseButton;
		}
	}

	const glm::uvec2 &InputSystem::InputBuffer::GetMousePosition() const
	{
		return mMousePosition;
	}

	int InputSystem::InputBuffer::GetMouseWheelDelta() const
	{
		return mMouseWheelDelta;
	}

	void InputSystem::InputBuffer::SetKey(Key keyCode, bool pressed)
	{
		mpKeys[(KeyCodesInt)keyCode] = pressed;
	}

	void InputSystem::InputBuffer::SetMouseButton(MouseButton button, MouseButtonState state)
	{
		if (button == MouseButton::LEFT_BUTTON)
		{
			mLeftMouseButton = state;
		}
		else if (button == MouseButton::MIDDLE_BUTTON)
		{
			mMiddleMouseButton = state;
		}
		else
		{
			mRightMouseButton = state;
		}
	}

	void InputSystem::InputBuffer::SetMousePosition(const glm::uvec2 &rPosition)
	{
		mMousePosition = rPosition;
	}

	void InputSystem::InputBuffer::IncrementMouseWheelDelta()
	{
		mMouseWheelDelta = MathI::Clamp(mMouseWheelDelta + 1, MINIMUM_MOUSE_WHEEL_DELTA, MAXIMUM_MOUSE_WHEEL_DELTA);
	}

	void InputSystem::InputBuffer::DecrementMouseWheelDelta()
	{
		mMouseWheelDelta = MathI::Clamp(mMouseWheelDelta - 1, MINIMUM_MOUSE_WHEEL_DELTA, MAXIMUM_MOUSE_WHEEL_DELTA);
	}

	void InputSystem::InputBuffer::Copy(InputBuffer &rOther)
	{
		memcpy(mpKeys, rOther.mpKeys, sizeof(mpKeys));
		mLeftMouseButton = rOther.mLeftMouseButton;
		mMiddleMouseButton = rOther.mMiddleMouseButton;
		mRightMouseButton = rOther.mRightMouseButton;
		mMousePosition = rOther.mMousePosition;
		mMouseWheelDelta = rOther.mMouseWheelDelta;
	}

	void InputSystem::InputBuffer::Clear()
	{
		mMouseWheelDelta = 0;
	}

}
