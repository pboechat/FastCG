#include <FastCG/Core/Log.h>
#include <FastCG/Input/InputSystem.h>
#include <FastCG/Platform/Application.h>
#include <FastCG/UI/ImGuiRenderer.h>
#include <FastCG/UI/ImGuiSystem.h>

#include <ImGuizmo.h>
#include <imgui.h>

namespace
{
    ImGuiKey GetImGuiKey(FastCG::Key key)
    {
        switch (key)
        {
        case FastCG::Key::BACKSPACE:
            return ImGuiKey_Backspace;
        case FastCG::Key::RETURN:
            return ImGuiKey_Enter;
        case FastCG::Key::ESCAPE:
            return ImGuiKey_Escape;
        case FastCG::Key::SPACE:
            return ImGuiKey_Space;
        case FastCG::Key::COMMA:
            return ImGuiKey_Comma;
        case FastCG::Key::MINUS:
            return ImGuiKey_Minus;
        case FastCG::Key::DOT:
            return ImGuiKey_Period;
        case FastCG::Key::SLASH:
            return ImGuiKey_Slash;
        case FastCG::Key::NUMBER_0:
            return ImGuiKey_0;
        case FastCG::Key::NUMBER_1:
            return ImGuiKey_1;
        case FastCG::Key::NUMBER_2:
            return ImGuiKey_2;
        case FastCG::Key::NUMBER_3:
            return ImGuiKey_3;
        case FastCG::Key::NUMBER_4:
            return ImGuiKey_4;
        case FastCG::Key::NUMBER_5:
            return ImGuiKey_5;
        case FastCG::Key::NUMBER_6:
            return ImGuiKey_6;
        case FastCG::Key::NUMBER_7:
            return ImGuiKey_7;
        case FastCG::Key::NUMBER_8:
            return ImGuiKey_8;
        case FastCG::Key::NUMBER_9:
            return ImGuiKey_9;
        case FastCG::Key::SEMI_COLON:
            return ImGuiKey_Semicolon;
        case FastCG::Key::EQUALS:
            return ImGuiKey_Equal;
        case FastCG::Key::BACKSLASH:
            return ImGuiKey_Backslash;
        case FastCG::Key::OPENING_SQUARE_BRACKET:
            return ImGuiKey_LeftBracket;
        case FastCG::Key::CLOSING_SQUARE_BRACKET:
            return ImGuiKey_RightBracket;
        case FastCG::Key::LETTER_A:
            return ImGuiKey_A;
        case FastCG::Key::LETTER_B:
            return ImGuiKey_B;
        case FastCG::Key::LETTER_C:
            return ImGuiKey_C;
        case FastCG::Key::LETTER_D:
            return ImGuiKey_D;
        case FastCG::Key::LETTER_E:
            return ImGuiKey_E;
        case FastCG::Key::LETTER_F:
            return ImGuiKey_F;
        case FastCG::Key::LETTER_G:
            return ImGuiKey_G;
        case FastCG::Key::LETTER_H:
            return ImGuiKey_H;
        case FastCG::Key::LETTER_I:
            return ImGuiKey_I;
        case FastCG::Key::LETTER_J:
            return ImGuiKey_J;
        case FastCG::Key::LETTER_K:
            return ImGuiKey_K;
        case FastCG::Key::LETTER_L:
            return ImGuiKey_L;
        case FastCG::Key::LETTER_M:
            return ImGuiKey_M;
        case FastCG::Key::LETTER_N:
            return ImGuiKey_N;
        case FastCG::Key::LETTER_O:
            return ImGuiKey_O;
        case FastCG::Key::LETTER_P:
            return ImGuiKey_P;
        case FastCG::Key::LETTER_Q:
            return ImGuiKey_Q;
        case FastCG::Key::LETTER_R:
            return ImGuiKey_R;
        case FastCG::Key::LETTER_S:
            return ImGuiKey_S;
        case FastCG::Key::LETTER_T:
            return ImGuiKey_T;
        case FastCG::Key::LETTER_U:
            return ImGuiKey_U;
        case FastCG::Key::LETTER_V:
            return ImGuiKey_V;
        case FastCG::Key::LETTER_W:
            return ImGuiKey_W;
        case FastCG::Key::LETTER_X:
            return ImGuiKey_X;
        case FastCG::Key::LETTER_Y:
            return ImGuiKey_Y;
        case FastCG::Key::LETTER_Z:
            return ImGuiKey_Z;
        case FastCG::Key::DEL:
            return ImGuiKey_Delete;
        case FastCG::Key::UNKNOWN:
            return ImGuiKey_None;
        case FastCG::Key::LEFT_ARROW:
            return ImGuiKey_LeftArrow;
        case FastCG::Key::UP_ARROW:
            return ImGuiKey_UpArrow;
        case FastCG::Key::RIGHT_ARROW:
            return ImGuiKey_RightArrow;
        case FastCG::Key::DOWN_ARROW:
            return ImGuiKey_DownArrow;
        case FastCG::Key::F1:
            return ImGuiKey_F1;
        case FastCG::Key::F2:
            return ImGuiKey_F2;
        case FastCG::Key::F3:
            return ImGuiKey_F3;
        case FastCG::Key::F4:
            return ImGuiKey_F4;
        case FastCG::Key::F5:
            return ImGuiKey_F5;
        case FastCG::Key::F6:
            return ImGuiKey_F6;
        case FastCG::Key::F7:
            return ImGuiKey_F7;
        case FastCG::Key::F8:
            return ImGuiKey_F8;
        case FastCG::Key::F9:
            return ImGuiKey_F9;
        case FastCG::Key::F10:
            return ImGuiKey_F10;
        case FastCG::Key::F11:
            return ImGuiKey_F11;
        case FastCG::Key::F12:
            return ImGuiKey_F12;
        case FastCG::Key::PAGE_UP:
            return ImGuiKey_PageUp;
        case FastCG::Key::PAGE_DOWN:
            return ImGuiKey_PageDown;
        case FastCG::Key::END:
            return ImGuiKey_End;
        case FastCG::Key::HOME:
            return ImGuiKey_Home;
        case FastCG::Key::INSERT:
            return ImGuiKey_Insert;
        case FastCG::Key::SHIFT:
            return ImGuiKey_LeftShift;
        case FastCG::Key::CONTROL:
            return ImGuiKey_LeftCtrl;
        case FastCG::Key::NUMLOCK:
            return ImGuiKey_NumLock;
        case FastCG::Key::KEYPAD_0:
            return ImGuiKey_Keypad0;
        case FastCG::Key::KEYPAD_1:
            return ImGuiKey_Keypad1;
        case FastCG::Key::KEYPAD_2:
            return ImGuiKey_Keypad2;
        case FastCG::Key::KEYPAD_3:
            return ImGuiKey_Keypad3;
        case FastCG::Key::KEYPAD_4:
            return ImGuiKey_Keypad4;
        case FastCG::Key::KEYPAD_5:
            return ImGuiKey_Keypad5;
        case FastCG::Key::KEYPAD_6:
            return ImGuiKey_Keypad6;
        case FastCG::Key::KEYPAD_7:
            return ImGuiKey_Keypad7;
        case FastCG::Key::KEYPAD_8:
            return ImGuiKey_Keypad8;
        case FastCG::Key::KEYPAD_9:
            return ImGuiKey_Keypad9;
        case FastCG::Key::KEYPAD_DECIMAL:
            return ImGuiKey_KeypadDecimal;
        case FastCG::Key::KEYPAD_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case FastCG::Key::KEYPAD_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case FastCG::Key::KEYPAD_SUBTRACT:
            return ImGuiKey_KeypadSubtract;
        case FastCG::Key::KEYPAD_ADD:
            return ImGuiKey_KeypadAdd;
        default:
            return ImGuiKey_None;
        }
    }
}

namespace FastCG
{
    ImGuiSystem::ImGuiSystem(const ImGuiSystemArgs &rArgs) : mArgs(rArgs)
    {
    }

    ImGuiSystem::~ImGuiSystem() = default;

    void ImGuiSystem::Initialize()
    {
        FASTCG_LOG_DEBUG(ImGuiSystem, "\tInitializing ImGui system");

        ImGui::CreateContext();

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void ImGuiSystem::BeginFrame(double deltaTime, KeyChange keyChanges[KEY_COUNT])
    {
        auto &rIo = ImGui::GetIO();

        rIo.DisplaySize.x = (float)mArgs.rScreenWidth;
        rIo.DisplaySize.y = (float)mArgs.rScreenHeight;

        ImGuizmo::SetRect(0, 0, rIo.DisplaySize.x, rIo.DisplaySize.y);

        rIo.DeltaTime =
            (float)deltaTime + 0.0000001f; // avoid an ImGui assert in the very unlikely event that deltaTime is zero!

        auto mousePos = InputSystem::GetMousePosition();
        rIo.AddMousePosEvent((float)mousePos.x, (float)mousePos.y);
        rIo.AddMouseButtonEvent(0, InputSystem::GetMouseButton((MouseButton)0) == MouseButtonState::PRESSED);
        rIo.AddMouseButtonEvent(1, InputSystem::GetMouseButton((MouseButton)2) == MouseButtonState::PRESSED);
        rIo.AddMouseButtonEvent(2, InputSystem::GetMouseButton((MouseButton)1) == MouseButtonState::PRESSED);
        rIo.AddMouseWheelEvent(0.0f, InputSystem::GetMouseWheelDelta());

        for (KeyInt i = 0; i < KEY_COUNT; ++i)
        {
            if (keyChanges[i] != KeyChange::NONE)
            {
                auto key = (Key)i;
                auto imguiKey = GetImGuiKey(key);
                bool down = ((KeyChangeInt)keyChanges[i] - 1);
                rIo.AddKeyEvent(imguiKey, down);
                if (down && IsCharKey(key))
                {
                    rIo.AddInputCharacter((unsigned int)key);
                }
            }
        }

        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiSystem::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImGuiSystem::Finalize()
    {
        ImGui::DestroyContext();
    }

}