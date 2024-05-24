#include <FastCG/Platform/Windows/WindowsApplication.h>

#ifdef FASTCG_WINDOWS

#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Platform/Timer.h>

#include <unordered_map>

namespace
{
#ifdef MAP_KEY
#undef MAP_KEY
#endif

#define MAP_KEY(a, b) {(uint64_t)a, FastCG::Key::b}

    const std::unordered_map<uint64_t, FastCG::Key> VIRTUAL_KEY_LUT = {MAP_KEY(VK_LEFT, LEFT_ARROW),
                                                                       MAP_KEY(VK_UP, UP_ARROW),
                                                                       MAP_KEY(VK_RIGHT, RIGHT_ARROW),
                                                                       MAP_KEY(VK_DOWN, DOWN_ARROW),
                                                                       MAP_KEY(VK_F1, F1),
                                                                       MAP_KEY(VK_F2, F2),
                                                                       MAP_KEY(VK_F3, F3),
                                                                       MAP_KEY(VK_F4, F4),
                                                                       MAP_KEY(VK_F5, F5),
                                                                       MAP_KEY(VK_F6, F6),
                                                                       MAP_KEY(VK_F7, F7),
                                                                       MAP_KEY(VK_F8, F8),
                                                                       MAP_KEY(VK_F9, F9),
                                                                       MAP_KEY(VK_F10, F10),
                                                                       MAP_KEY(VK_F11, F11),
                                                                       MAP_KEY(VK_F12, F12),
                                                                       MAP_KEY(VK_PRIOR, PAGE_UP),
                                                                       MAP_KEY(VK_NEXT, PAGE_DOWN),
                                                                       MAP_KEY(VK_END, END),
                                                                       MAP_KEY(VK_HOME, HOME),
                                                                       MAP_KEY(VK_INSERT, INSERT),
                                                                       MAP_KEY(VK_NUMLOCK, NUMLOCK),
                                                                       MAP_KEY(VK_NUMPAD0, KEYPAD_0),
                                                                       MAP_KEY(VK_NUMPAD1, KEYPAD_1),
                                                                       MAP_KEY(VK_NUMPAD2, KEYPAD_2),
                                                                       MAP_KEY(VK_NUMPAD3, KEYPAD_3),
                                                                       MAP_KEY(VK_NUMPAD4, KEYPAD_4),
                                                                       MAP_KEY(VK_NUMPAD5, KEYPAD_5),
                                                                       MAP_KEY(VK_NUMPAD6, KEYPAD_6),
                                                                       MAP_KEY(VK_NUMPAD7, KEYPAD_7),
                                                                       MAP_KEY(VK_NUMPAD8, KEYPAD_8),
                                                                       MAP_KEY(VK_NUMPAD9, KEYPAD_9),
                                                                       MAP_KEY(VK_DECIMAL, KEYPAD_DECIMAL),
                                                                       MAP_KEY(VK_DIVIDE, KEYPAD_DIVIDE),
                                                                       MAP_KEY(VK_MULTIPLY, KEYPAD_MULTIPLY),
                                                                       MAP_KEY(VK_SUBTRACT, KEYPAD_SUBTRACT),
                                                                       MAP_KEY(VK_ADD, KEYPAD_ADD),
                                                                       MAP_KEY(VK_SHIFT, SHIFT),
                                                                       MAP_KEY(VK_CONTROL, CONTROL)};

#undef MAP_KEY

    bool TranslateToVirtualKey(uint64_t virtualKey, FastCG::Key &rKey)
    {
        auto it = VIRTUAL_KEY_LUT.find(virtualKey);
        if (it == VIRTUAL_KEY_LUT.end())
        {
            return false;
        }
        rKey = it->second;
        return true;
    }
}

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // FIXME: don't use static storage to store temporary values (side-effects!)
    static FastCG::Key sLastKeyDown = FastCG::Key::UNKNOWN;

    auto *pApplication = FastCG::WindowsApplication::GetInstance();
    switch (uMsg)
    {
    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        pApplication->WindowResizeCallback((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
        break;
    case WM_LBUTTONDOWN:
        pApplication->MouseButtonCallback(FastCG::MouseButton::LEFT_BUTTON, FastCG::MouseButtonState::PRESSED);
        break;
    case WM_RBUTTONDOWN:
        pApplication->MouseButtonCallback(FastCG::MouseButton::RIGHT_BUTTON, FastCG::MouseButtonState::PRESSED);
        break;
    case WM_MBUTTONDOWN:
        pApplication->MouseButtonCallback(FastCG::MouseButton::MIDDLE_BUTTON, FastCG::MouseButtonState::PRESSED);
        break;
    case WM_LBUTTONUP:
        pApplication->MouseButtonCallback(FastCG::MouseButton::LEFT_BUTTON, FastCG::MouseButtonState::RELEASED);
        break;
    case WM_RBUTTONUP:
        pApplication->MouseButtonCallback(FastCG::MouseButton::RIGHT_BUTTON, FastCG::MouseButtonState::RELEASED);
        break;
    case WM_MBUTTONUP:
        pApplication->MouseButtonCallback(FastCG::MouseButton::MIDDLE_BUTTON, FastCG::MouseButtonState::RELEASED);
        break;
    case WM_MOUSEMOVE:
        pApplication->MouseMoveCallback((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
        break;
    case WM_KEYDOWN: {
        if (sLastKeyDown == FastCG::Key::UNKNOWN && MapVirtualKey((UINT)wParam, MAPVK_VK_TO_CHAR) == 0)
        {
            FastCG::Key key;
            if (TranslateToVirtualKey((uint64_t)wParam, key))
            {
                pApplication->KeyboardCallback(key, true);
                sLastKeyDown = key;
            }
        }
    }
    break;
    case WM_KEYUP:
        if (sLastKeyDown != FastCG::Key::UNKNOWN)
        {
            pApplication->KeyboardCallback(sLastKeyDown, false);
            sLastKeyDown = FastCG::Key::UNKNOWN;
        }
        break;
    case WM_CHAR: {
        char asciiChars[2] = {0};
        if (sLastKeyDown == FastCG::Key::UNKNOWN &&
            WideCharToMultiByte(CP_ACP, 0, (LPCWCH)&wParam, 1, asciiChars, sizeof(asciiChars), nullptr, nullptr))
        {
            auto key = (FastCG::Key)asciiChars[0];
            if (IsCharKey(key))
            {
                pApplication->KeyboardCallback(key, true);
                sLastKeyDown = key;
            }
        }
    }
    break;
    case WM_MOUSEWHEEL:
        pApplication->MouseWheelCallback(GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace FastCG
{
    void WindowsApplication::OnPreInitialize()
    {
        BaseApplication::OnPreInitialize();

        mInstance = GetModuleHandle(NULL);

        RECT windowRect;
        windowRect.left = (LONG)0;
        windowRect.right = (LONG)GetScreenWidth();
        windowRect.top = (LONG)0;
        windowRect.bottom = (LONG)GetScreenHeight();

        WNDCLASSEX windowClass;
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WndProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = mInstance;
        windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = NULL;
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = "FastCG_Window";
        windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

        if (!RegisterClassEx(&windowClass))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't register window class");
        }

        auto dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        auto dwStyle = WS_OVERLAPPEDWINDOW;
        AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
        mHWnd = CreateWindowEx(NULL, "FastCG_Window", GetWindowTitle().c_str(),
                               dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, windowRect.right - windowRect.left,
                               windowRect.bottom - windowRect.top, NULL, NULL, mInstance, NULL);

        if (mHWnd == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't create window");
        }

        ShowWindow(mHWnd, SW_SHOW);
        UpdateWindow(mHWnd);
    }

    void WindowsApplication::OnPostFinalize()
    {
        if (mHWnd != 0)
        {
            DestroyWindow(mHWnd);
            mHWnd = 0;
        }

        BaseApplication::OnPostFinalize();
    }

    void WindowsApplication::RunMainLoop()
    {
        MSG msg;
        while (mRunning)
        {
            auto osStart = Timer::GetTime();

            while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if (!GetMessage(&msg, NULL, 0, 0))
                {
                    mRunning = false;
                    goto __exit;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            RunMainLoopIteration(Timer::GetTime() - osStart);
        }
    __exit:
        return;
    }

}

#endif