#include <FastCG/Input/MouseButton.h>
#include <FastCG/Input/InputSystem.h>
#include <FastCG/Core/Math.h>

#include <cstring>

namespace FastCG
{
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

	void InputSystem::InputBuffer::SetKey(Key keyCode, bool pressed)
	{
		mpKeys[(KeyInt)keyCode] = pressed;
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
		std::memcpy(mpKeys, rOther.mpKeys, sizeof(mpKeys));
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
