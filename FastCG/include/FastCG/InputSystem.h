#ifndef FASTCG_INPUT_H
#define FASTCG_INPUT_H

#include <FastCG/System.h>
#include <FastCG/MouseButton.h>
#include <FastCG/Key.h>

#include <glm/glm.hpp>

namespace FastCG
{
	class BaseApplication;

	struct InputSystemArgs
	{
	};

	class InputSystem
	{
		FASTCG_DECLARE_SYSTEM(InputSystem, InputSystemArgs);

	public:
		static const int MINIMUM_MOUSE_WHEEL_DELTA = -128;
		static const int MOUSE_WHEEL_STEP = 1;
		static const int MAXIMUM_MOUSE_WHEEL_DELTA = 128;

		static bool GetKey(Key keyCode);
		static MouseButtonState GetMouseButton(MouseButton button);
		static const glm::uvec2 &GetMousePosition();
		static int GetMouseWheelDelta();

		template <typename MaskType, typename ElementType, typename CallbackType>
		static void IsKeyPressed(Key keyCode, MaskType &pressedElementsMask, ElementType element, const CallbackType &callback)
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

	private:
		class InputBuffer
		{
		public:
			InputBuffer();

			bool GetKey(Key keyCode) const;
			MouseButtonState GetMouseButton(MouseButton button) const;
			const glm::uvec2 &GetMousePosition() const;
			int GetMouseWheelDelta() const;
			void SetKey(Key keyCode, bool state);
			void SetMouseButton(MouseButton button, MouseButtonState state);
			void SetMousePosition(const glm::uvec2 &rPosition);
			void IncrementMouseWheelDelta();
			void DecrementMouseWheelDelta();
			void Copy(InputBuffer &rOther);
			void Clear();

		private:
			bool mpKeys[std::numeric_limits<KeyCodesInt>::max()];
			MouseButtonState mLeftMouseButton{MouseButtonState::RELEASED};
			MouseButtonState mMiddleMouseButton{MouseButtonState::RELEASED};
			MouseButtonState mRightMouseButton{MouseButtonState::RELEASED};
			glm::uvec2 mMousePosition;
			int mMouseWheelDelta{0};
		};

		InputBuffer mFrontBuffer;
		InputBuffer mBackBuffer;

		InputSystem(const InputSystemArgs &rArgs) {}

		void SetKey(Key keyCode, bool state);
		void SetMouseButton(MouseButton button, MouseButtonState state);
		void SetMousePosition(const glm::uvec2 &rPosition);
		void IncrementMouseWheelDelta();
		void DecrementMouseWheelDelta();
		void Swap();

		friend class BaseApplication;
	};

}

#endif