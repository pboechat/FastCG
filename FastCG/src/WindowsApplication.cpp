#include <FastCG/WindowsApplication.h>

#ifdef FASTCG_WINDOWS

#include <FastCG/Graphics/GraphicsSystem.h>

#include <unordered_map>

namespace
{
#define KEY(a, b)                    \
	{                                \
		(uint64_t) b, FastCG::Key::a \
	}

	std::unordered_map<uint64_t, FastCG::Key> gKeyLut = {
		KEY(BACKSPACE, VK_BACK),
		KEY(RETURN, VK_RETURN),
		KEY(ESCAPE, VK_ESCAPE),
		KEY(SPACE, VK_SPACE),
		KEY(ASTERISK, VK_MULTIPLY),
		KEY(PLUS, VK_ADD),
		KEY(COMMA, VK_OEM_COMMA),
		KEY(MINUS, VK_SUBTRACT),
		KEY(DOT, VK_OEM_PERIOD),
		KEY(SLASH, VK_DIVIDE),
		KEY(NUMBER_0, '0'),
		KEY(NUMBER_1, '1'),
		KEY(NUMBER_2, '2'),
		KEY(NUMBER_3, '3'),
		KEY(NUMBER_4, '4'),
		KEY(NUMBER_5, '4'),
		KEY(NUMBER_6, '5'),
		KEY(NUMBER_7, '6'),
		KEY(NUMBER_8, '7'),
		KEY(NUMBER_9, '8'),
		KEY(COLON, 58),
		KEY(SEMI_COLON, 59),
		KEY(EQUALS, 61),
		KEY(LEFT_ARROW, VK_LEFT),
		KEY(UP_ARROW, VK_UP),
		KEY(RIGHT_ARROW, VK_RIGHT),
		KEY(DOWN_ARROW, VK_DOWN),
		KEY(F1, VK_F1),
		KEY(F2, VK_F2),
		KEY(F3, VK_F3),
		KEY(F4, VK_F4),
		KEY(F5, VK_F5),
		KEY(F6, VK_F6),
		KEY(F7, VK_F7),
		KEY(F8, VK_F8),
		KEY(F9, VK_F9),
		KEY(F10, VK_F10),
		KEY(F11, VK_F11),
		KEY(F12, VK_F12),
		KEY(PAGE_UP, VK_PRIOR),
		KEY(PAGE_DOWN, VK_NEXT),
		KEY(END, VK_END),
		KEY(HOME, VK_HOME),
		KEY(INSERT, VK_INSERT),
		KEY(SHIFT, VK_SHIFT),
		KEY(CONTROL, VK_CONTROL),
		KEY(OPEN_SQUARE_BRACKET, 91),
		KEY(BACKSLASH, 92),
		KEY(CLOSE_SQUARE_BRACKET, 93),
		KEY(LETTER_A, 'A'),
		KEY(LETTER_B, 'B'),
		KEY(LETTER_C, 'C'),
		KEY(LETTER_D, 'D'),
		KEY(LETTER_E, 'E'),
		KEY(LETTER_F, 'F'),
		KEY(LETTER_G, 'G'),
		KEY(LETTER_H, 'H'),
		KEY(LETTER_I, 'I'),
		KEY(LETTER_J, 'J'),
		KEY(LETTER_K, 'K'),
		KEY(LETTER_L, 'L'),
		KEY(LETTER_M, 'M'),
		KEY(LETTER_N, 'N'),
		KEY(LETTER_O, 'O'),
		KEY(LETTER_P, 'P'),
		KEY(LETTER_Q, 'Q'),
		KEY(LETTER_R, 'R'),
		KEY(LETTER_S, 'S'),
		KEY(LETTER_T, 'T'),
		KEY(LETTER_U, 'U'),
		KEY(LETTER_V, 'V'),
		KEY(LETTER_W, 'W'),
		KEY(LETTER_X, 'X'),
		KEY(LETTER_Y, 'Y'),
		KEY(LETTER_Z, 'Z'),
		KEY(TILDE, 126),
		KEY(DEL, VK_DELETE),
	};

	FastCG::Key TranslateKey(uint64_t key)
	{
		auto it = gKeyLut.find(key);
		if (it == gKeyLut.end())
		{
			return FastCG::Key::UNKNOWN;
		}
		return it->second;
	}
}

namespace FastCG
{
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto *app = WindowsApplication::GetInstance();
		switch (uMsg)
		{
		case WM_DESTROY:
		case WM_QUIT:
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			app->WindowResizeCallback((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			break;
		case WM_LBUTTONDOWN:
			app->MouseButtonCallback(MouseButton::LEFT_BUTTON, MouseButtonState::PRESSED);
			break;
		case WM_RBUTTONDOWN:
			app->MouseButtonCallback(MouseButton::RIGHT_BUTTON, MouseButtonState::PRESSED);
			break;
		case WM_MBUTTONDOWN:
			app->MouseButtonCallback(MouseButton::MIDDLE_BUTTON, MouseButtonState::PRESSED);
			break;
		case WM_LBUTTONUP:
			app->MouseButtonCallback(MouseButton::LEFT_BUTTON, MouseButtonState::RELEASED);
			break;
		case WM_RBUTTONUP:
			app->MouseButtonCallback(MouseButton::RIGHT_BUTTON, MouseButtonState::RELEASED);
			break;
		case WM_MBUTTONUP:
			app->MouseButtonCallback(MouseButton::MIDDLE_BUTTON, MouseButtonState::RELEASED);
			break;
		case WM_MOUSEMOVE:
			app->MouseMoveCallback((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			app->KeyboardCallback(TranslateKey((uint64_t)wParam), uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
			break;
		default:
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void WindowsApplication::RunMainLoop()
	{
		MSG msg;
		while (mRunning)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage(&msg, NULL, 0, 0))
				{
					mRunning = false;
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			RunMainLoopIteration();
		}
	}

	HDC WindowsApplication::GetDeviceContext()
	{
		if (mHDC != 0)
		{
			return mHDC;
		}

		CreateAndSetupDeviceContext();

		return mHDC;
	}

	HWND WindowsApplication::GetWindow()
	{
		if (mHWnd != 0)
		{
			return mHWnd;
		}

		CreateWindow_();

		return mHWnd;
	}

	void WindowsApplication::CreateAndSetupDeviceContext()
	{
		auto hWnd = GetWindow();

		mHDC = GetDC(hWnd);

		GraphicsSystem::GetInstance()->SetupPixelFormat();

		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	void WindowsApplication::CreateWindow_()
	{
		mHInstance = GetModuleHandle(NULL);

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
		windowClass.hInstance = mHInstance;
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = "FastCG_Window";
		windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&windowClass))
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error registering window class");
		}

		auto dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		auto dwStyle = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
		mHWnd = CreateWindowEx(NULL,
							   "FastCG_Window",
							   GetWindowTitle().c_str(),
							   dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
							   0,
							   0,
							   windowRect.right - windowRect.left,
							   windowRect.bottom - windowRect.top,
							   NULL,
							   NULL,
							   mHInstance,
							   NULL);

		if (mHWnd == 0)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error creating window");
		}
	}

	void WindowsApplication::DestroyDeviceContext()
	{
		ReleaseDC(mHWnd, mHDC);
		mHDC = 0;
	}

	uint64_t WindowsApplication::GetNativeKey(Key key) const
	{
		for (auto it = gKeyLut.cbegin(); it != gKeyLut.cend(); ++it)
		{
			if (it->second == key)
			{
				return it->first;
			}
		}
		return uint64_t(~0);
	}
}

#endif