#ifndef FASTCG_INPUT_H_
#define FASTCG_INPUT_H_

#include <FastCG/MouseButton.h>
#include <FastCG/Application.h>

#include <glm/glm.hpp>

namespace FastCG
{
	class Input
	{
	public:
		static const int NUMBER_OF_KEYS = 128;
		static const int MINIMUM_MOUSE_WHEEL_DELTA = -128;
		static const int MOUSE_WHEEL_STEP = 1;
		static const int MAXIMUM_MOUSE_WHEEL_DELTA = 128;

		Input();
		~Input();

		static bool GetKey(int keyCode);
		static MouseButtonState GetMouseButton(MouseButton button);
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
			MouseButtonState GetMouseButton(MouseButton button) const;
			const glm::vec2& GetMousePosition() const;
			int GetMouseWheelDelta() const;

			void SetKey(int keyCode, bool state);
			void SetMouseButton(MouseButton button, MouseButtonState state);
			void SetMousePosition(const glm::vec2& rPosition);
			void IncrementMouseWheelDelta();
			void DecrementMouseWheelDelta();

			void Copy(InputBuffer& rOther);
			void Clear();

		private:
			bool* mpKeys;
			MouseButtonState mLeftMouseButton{ MouseButtonState::RELEASED };
			MouseButtonState mMiddleMouseButton{ MouseButtonState::RELEASED };
			MouseButtonState mRightMouseButton{ MouseButtonState::RELEASED };
			glm::vec2 mMousePosition;
			int mMouseWheelDelta{ 0 };

		};

		static Input* s_mpInstance;

		InputBuffer mFrontBuffer;
		InputBuffer mBackBuffer;

		void SetKey(int keyCode, bool state);
		void SetMouseButton(MouseButton button, MouseButtonState state);
		void SetMousePosition(const glm::vec2& rPosition);
		void IncrementMouseWheelDelta();
		void DecrementMouseWheelDelta();
		void Swap();

	};

}

#endif