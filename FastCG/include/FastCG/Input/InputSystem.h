#ifndef FASTCG_INPUT_H
#define FASTCG_INPUT_H

#include <FastCG/Core/Macros.h>
#include <FastCG/Core/System.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Input/MouseButton.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <type_traits>

namespace FastCG
{
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
        inline static void GetKeyChanges(KeyChange keyChanges[KEY_COUNT])
        {
            const auto *pBackBufferKeys = GetInstance()->mBackBuffer.GetKeys();
            const auto *pFrontBufferKeys = GetInstance()->mFrontBuffer.GetKeys();
            for (KeyInt i = 0; i < KEY_COUNT; ++i)
            {
                if (pBackBufferKeys[i] != pFrontBufferKeys[i])
                {
                    keyChanges[i] = pBackBufferKeys[i] ? KeyChange::PRESSED : KeyChange::RELEASED;
                }
                else
                {
                    keyChanges[i] = KeyChange::NONE;
                }
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
        inline static float GetMouseWheelDelta()
        {
            return GetInstance()->mFrontBuffer.GetMouseWheelDelta();
        }
        template <typename MaskType, typename ElementType, typename CallbackType>
        static void IsKeyPressed(Key keyCode, MaskType &pressedElementsMask, ElementType element,
                                 const CallbackType &callback)
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
            inline float GetMouseWheelDelta() const
            {
                return mMouseWheelDelta;
            }
            void SetKey(Key keyCode, bool state);
            void SetMouseButton(MouseButton button, MouseButtonState state);
            void SetMousePosition(const glm::uvec2 &rPosition);
            void SetMouseWheelDelta(float delta);
            void Copy(InputBuffer &rOther);
            void Clear();

        private:
            bool mpKeys[KEY_COUNT];
            MouseButtonState mLeftMouseButton{MouseButtonState::RELEASED};
            MouseButtonState mMiddleMouseButton{MouseButtonState::RELEASED};
            MouseButtonState mRightMouseButton{MouseButtonState::RELEASED};
            glm::uvec2 mMousePosition;
            float mMouseWheelDelta{0};
        };

        InputBuffer mFrontBuffer;
        InputBuffer mBackBuffer;

        InputSystem(const InputSystemArgs &rArgs)
        {
            FASTCG_UNUSED(rArgs);
        }

        void SetKey(Key keyCode, bool state);
        void SetMouseButton(MouseButton button, MouseButtonState state);
        void SetMousePosition(const glm::uvec2 &rPosition);
        void SetMouseWheelDelta(float delta);
        void Swap();
    };

}

#endif