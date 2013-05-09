#include <Input.h>
#include <Exception.h>
#include <MouseButton.h>
#include <MathT.h>

#include <memory>

Input* Input::s_mpInstance = 0;

Input::Input()
{
	if (s_mpInstance != 0)
	{
		THROW_EXCEPTION(Exception, "There can only be one Input instance");
	}

	s_mpInstance = this;
}

Input::~Input()
{
	s_mpInstance = 0;
}

bool Input::GetKey(int keyCode)
{
	return s_mpInstance->mFrontBuffer.GetKey(keyCode);
}

bool Input::GetMouseButton(int button)
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

void Input::SetKey(int keyCode, bool state)
{
	mBackBuffer.SetKey(keyCode, state);
}

void Input::SetMouseButton(int button, bool state)
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

Input::InputBuffer::InputBuffer() :
	mLeftMouseButton(false),
	mMiddleMouseButton(false),
	mRightMouseButton(false),
	mMouseWheelDelta(0)
{
	mpKeys = new bool[NUMBER_OF_KEYS];
	memset(mpKeys, 0, NUMBER_OF_KEYS * sizeof(bool));
}

Input::InputBuffer::~InputBuffer()
{
	delete[] mpKeys;
}

bool Input::InputBuffer::GetKey(int keyCode) const
{
	return mpKeys[keyCode];
}

bool Input::InputBuffer::GetMouseButton(int button) const
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

void Input::InputBuffer::SetKey(int keyCode, bool state)
{
	mpKeys[keyCode] = state;
}

void Input::InputBuffer::SetMouseButton(int button, bool state)
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
	glm::vec2 mousePosition = mMousePosition;
	mMousePosition = rOther.mMousePosition;
	mMouseWheelDelta = rOther.mMouseWheelDelta;
}

void Input::InputBuffer::Clear()
{
	mMouseWheelDelta = 0;
}
