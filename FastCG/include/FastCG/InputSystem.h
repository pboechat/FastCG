#ifndef FASTCG_INPUT_H
#define FASTCG_INPUT_H

#include <FastCG/MouseButton.h>
#include <FastCG/BaseApplication.h>

#include <glm/glm.hpp>

namespace FastCG
{
	struct InputSystemArgs
	{
	};

	class InputSystem
	{
		FASTCG_DECLARE_SYSTEM(InputSystem, InputSystemArgs);

	public:
		static const int NUMBER_OF_KEYS = 128;
		static const int MINIMUM_MOUSE_WHEEL_DELTA = -128;
		static const int MOUSE_WHEEL_STEP = 1;
		static const int MAXIMUM_MOUSE_WHEEL_DELTA = 128;

		static bool GetKey(int keyCode);
		static MouseButtonState GetMouseButton(MouseButton button);
		static const glm::vec2 &GetMousePosition();
		static int GetMouseWheelDelta();

		friend class BaseApplication;

	private:
		class InputBuffer
		{
		public:
			InputBuffer();

			bool GetKey(int keyCode) const;
			MouseButtonState GetMouseButton(MouseButton button) const;
			const glm::vec2 &GetMousePosition() const;
			int GetMouseWheelDelta() const;
			void SetKey(int keyCode, bool state);
			void SetMouseButton(MouseButton button, MouseButtonState state);
			void SetMousePosition(const glm::vec2 &rPosition);
			void IncrementMouseWheelDelta();
			void DecrementMouseWheelDelta();
			void Copy(InputBuffer &rOther);
			void Clear();

		private:
			bool mpKeys[NUMBER_OF_KEYS];
			MouseButtonState mLeftMouseButton{MouseButtonState::RELEASED};
			MouseButtonState mMiddleMouseButton{MouseButtonState::RELEASED};
			MouseButtonState mRightMouseButton{MouseButtonState::RELEASED};
			glm::vec2 mMousePosition;
			int mMouseWheelDelta{0};
		};

		InputBuffer mFrontBuffer;
		InputBuffer mBackBuffer;

		InputSystem(const InputSystemArgs &rArgs) {}
		virtual ~InputSystem() = default;

		void SetKey(int keyCode, bool state);
		void SetMouseButton(MouseButton button, MouseButtonState state);
		void SetMousePosition(const glm::vec2 &rPosition);
		void IncrementMouseWheelDelta();
		void DecrementMouseWheelDelta();
		void Swap();
	};

	template <typename MaskType, typename ElementType, typename CallbackType>
	void IsKeyPressed(int keyCode, MaskType &pressedElementsMask, ElementType element, const CallbackType &callback)
	{
		MaskType elementMask = 1 << element;
		if (InputSystem::GetKey(keyCode))
		{
			if ((pressedElementsMask & elementMask) == 0)
			{
				callback();
				pressedElementsMask |= elementMask;
			}
		}
		else
		{
			pressedElementsMask &= ~elementMask;
		}
	}

}

#endif