#ifndef INPUT_H__
#define INPUT_H__

#include <Application.h>

#include <glm/glm.hpp>

class Input
{
public:
	static const int NUMBER_OF_KEYS = 128;
	static const int MINIMUM_MOUSE_WHEEL_DELTA = -128;
	static const int MOUSE_WHEEL_STEP = 1;
	static const int MAXIMUM_MOUSE_WHEEL_DELTA = 128;

	static bool GetKey(int keyCode);
	static bool GetMouseButton(int button);
	static const glm::vec2& GetMousePosition();
	static int GetMouseWheelDelta();

	friend class Application;

private:
	class InputBuffer
	{
	public:
		InputBuffer();
		~InputBuffer();

		bool GetKey(int keyCode) const;
		bool GetMouseButton(int button) const;
		const glm::vec2& GetMousePosition() const;
		int GetMouseWheelDelta() const;

		void SetKey(int keyCode, bool state);
		void SetMouseButton(int button, bool state);
		void SetMousePosition(const glm::vec2& rPosition);
		void IncrementMouseWheelDelta();
		void DecrementMouseWheelDelta();

		void Copy(InputBuffer& rOther);
		void Clear();

	private:
		bool* mpKeys;
		bool mLeftMouseButton;
		bool mMiddleMouseButton;
		bool mRightMouseButton;
		glm::vec2 mMousePosition;
		int mMouseWheelDelta;

	};

	static Input* s_mpInstance;

	InputBuffer mFrontBuffer;
	InputBuffer mBackBuffer;

	Input();
	~Input();

	void SetKey(int keyCode, bool state);
	void SetMouseButton(int button, bool state);
	void SetMousePosition(const glm::vec2& rPosition);
	void IncrementMouseWheelDelta();
	void DecrementMouseWheelDelta();
	void Swap();

};

#endif