#ifndef FASTCG_INPUT_H
#define FASTCG_INPUT_H

#include <FastCG/System.h>
#include <FastCG/MouseButton.h>
#include <FastCG/Key.h>

#include <glm/glm.hpp>

#include <type_traits>

namespace FastCG
{
	class BaseApplication;

	struct InputSystemArgs
	{
	};

	enum class KeyChange : uint8_t
	{
		NONE = 0,
		RELEASED,
		PRESSED,
	};

	using KeyChangeInt = std::underlying_type_t<KeyChange>;

	class InputSystem
	{
		FASTCG_DECLARE_SYSTEM(InputSystem, InputSystemArgs);

	public:
		static const int MINIMUM_MOUSE_WHEEL_DELTA = -128;
		static const int MOUSE_WHEEL_STEP = 1;
		static const int MAXIMUM_MOUSE_WHEEL_DELTA = 128;

		inline static void GetKeyChanges(KeyChange keyChanges[KEY_COUNT])
		{
			const auto *pBackBufferKeys = GetInstance()->mBackBuffer.GetKeys();
			const auto *pFrontBufferKeys = GetInstance()->mFrontBuffer.GetKeys();
			for (KeyInt i = 0; i < KEY_COUNT; ++i)
			{
				keyChanges[i] = (KeyChange)((pBackBufferKeys[i] ^ pFrontBufferKeys[i]) & (1 << (KeyChangeInt)pBackBufferKeys[i]));
			}
		}
		inline static bool GetKey(Key keyCode)
		{
			return GetInstance()->mFrontBuffer.GetKey(keyCode);
		}
		inline static MouseButtonState GetMouseButton(MouseButton button)
		{
			return GetInstance()->mFrontBuffer.GetMouseButton(button);
		}
		inline static const glm::uvec2 &GetMousePosition()
		{
			return GetInstance()->mFrontBuffer.GetMousePosition();
		}
		inline static int GetMouseWheelDelta()
		{
			return GetInstance()->mFrontBuffer.GetMouseWheelDelta();
		}

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

			inline bool GetKey(Key keyCode) const
			{
				return mpKeys[(KeyInt)keyCode];
			}
			inline const bool *GetKeys() const
			{
				return mpKeys;
			}
			inline MouseButtonState GetMouseButton(MouseButton button) const
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
			inline const glm::uvec2 &GetMousePosition() const
			{
				return mMousePosition;
			}
			inline int GetMouseWheelDelta() const
			{
				return mMouseWheelDelta;
			}
			void SetKey(Key keyCode, bool state);
			void SetMouseButton(MouseButton button, MouseButtonState state);
			void SetMousePosition(const glm::uvec2 &rPosition);
			void IncrementMouseWheelDelta();
			void DecrementMouseWheelDelta();
			void Copy(InputBuffer &rOther);
			void Clear();

		private:
			bool mpKeys[KEY_COUNT];
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