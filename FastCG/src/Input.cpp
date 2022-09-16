#include <FastCG/MouseButton.h>
#include <FastCG/MathT.h>
#include <FastCG/Input.h>
#include <FastCG/Exception.h>

#include <memory>

namespace FastCG
{
	Input* Input::s_mpInstance = nullptr;

	Input::Input()
	{
		if (s_mpInstance != nullptr)
		{
			THROW_EXCEPTION(Exception, "There can only be one Input instance");
		}

		s_mpInstance = this;
	}

	Input::~Input()
	{
		s_mpInstance = nullptr;
	}

	bool Input::GetKey(int keyCode)
	{
		return s_mpInstance->mFrontBuffer.GetKey(keyCode);
	}

	MouseButtonState Input::GetMouseButton(MouseButton button)
	{
		return s_mpInstance->mFrontBuffer.GetMouseButton(button);
	}

	const glm::vec2& Input::GetMousePosition()
	{
		return s_mpInstance->mFrontBuffer.GetMousePosition();
	}

	int Input::GetMouseWheelDelta()
	{
		return s_mpInstance->mFrontBuffer.GetMouseWheelDelta();
	}

	void Input::SetKey(int keyCode, bool pressed)
	{
		mBackBuffer.SetKey(keyCode, pressed);
	}

	void Input::SetMouseButton(MouseButton button, MouseButtonState state)
	{
		mBackBuffer.SetMouseButton(button, state);
	}

	void Input::SetMousePosition(const glm::vec2& rPosition)
	{
		mBackBuffer.SetMousePosition(rPosition);
	}

	void Input::IncrementMouseWheelDelta()
	{
		mBackBuffer.IncrementMouseWheelDelta();
	}

	void Input::DecrementMouseWheelDelta()
	{
		mBackBuffer.DecrementMouseWheelDelta();
	}

	void Input::Swap()
	{
		mFrontBuffer.Copy(mBackBuffer);
		mBackBuffer.Clear();
	}

	Input::InputBuffer::InputBuffer()
	{
		mpKeys = new bool[NUMBER_OF_KEYS];
		memset(mpKeys, 0, NUMBER_OF_KEYS * sizeof(bool));
	}

	Input::InputBuffer::~InputBuffer()
	{
		if (mpKeys != 0)
		{
			delete[] mpKeys;
			mpKeys = 0;
		}
	}

	bool Input::InputBuffer::GetKey(int keyCode) const
	{
		return mpKeys[keyCode];
	}

	MouseButtonState Input::InputBuffer::GetMouseButton(MouseButton button) const
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

	const glm::vec2& Input::InputBuffer::GetMousePosition() const
	{
		return mMousePosition;
	}

	int Input::InputBuffer::GetMouseWheelDelta() const
	{
		return mMouseWheelDelta;
	}

	void Input::InputBuffer::SetKey(int keyCode, bool pressed)
	{
		mpKeys[keyCode] = pressed;
	}

	void Input::InputBuffer::SetMouseButton(MouseButton button, MouseButtonState state)
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

	void Input::InputBuffer::SetMousePosition(const glm::vec2& rPosition)
	{
		mMousePosition = rPosition;
	}

	void Input::InputBuffer::IncrementMouseWheelDelta()
	{
		mMouseWheelDelta = MathI::Clamp(mMouseWheelDelta + 1, MINIMUM_MOUSE_WHEEL_DELTA, MAXIMUM_MOUSE_WHEEL_DELTA);
	}

	void Input::InputBuffer::DecrementMouseWheelDelta()
	{
		mMouseWheelDelta = MathI::Clamp(mMouseWheelDelta - 1, MINIMUM_MOUSE_WHEEL_DELTA, MAXIMUM_MOUSE_WHEEL_DELTA);
	}

	void Input::InputBuffer::Copy(InputBuffer& rOther)
	{
		memcpy(mpKeys, rOther.mpKeys, NUMBER_OF_KEYS * sizeof(bool));
		mLeftMouseButton = rOther.mLeftMouseButton;
		mMiddleMouseButton = rOther.mMiddleMouseButton;
		mRightMouseButton = rOther.mRightMouseButton;
		auto mousePosition = mMousePosition;
		mMousePosition = rOther.mMousePosition;
		mMouseWheelDelta = rOther.mMouseWheelDelta;
	}

	void Input::InputBuffer::Clear()
	{
		mMouseWheelDelta = 0;
	}

}
