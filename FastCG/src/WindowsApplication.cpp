#include <FastCG/WindowsApplication.h>

#ifdef FASTCG_WINDOWS

namespace FastCG
{
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
        case WM_QUIT:
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            WindowsApplication::GetInstance()->WindowResizeCallback(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_LBUTTONDOWN:
            WindowsApplication::GetInstance()->MouseButtonCallback(MouseButton::LEFT_BUTTON, MouseButtonState::PRESSED, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_RBUTTONDOWN:
            WindowsApplication::GetInstance()->MouseButtonCallback(MouseButton::RIGHT_BUTTON, MouseButtonState::PRESSED, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_MBUTTONDOWN:
            WindowsApplication::GetInstance()->MouseButtonCallback(MouseButton::MIDDLE_BUTTON, MouseButtonState::PRESSED, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_LBUTTONUP:
            WindowsApplication::GetInstance()->MouseButtonCallback(MouseButton::LEFT_BUTTON, MouseButtonState::RELEASED, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_RBUTTONUP:
            WindowsApplication::GetInstance()->MouseButtonCallback(MouseButton::RIGHT_BUTTON, MouseButtonState::RELEASED, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_MBUTTONUP:
            WindowsApplication::GetInstance()->MouseButtonCallback(MouseButton::MIDDLE_BUTTON, MouseButtonState::RELEASED, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_MOUSEMOVE:
            WindowsApplication::GetInstance()->MouseMoveCallback(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_KEYDOWN:
            WindowsApplication::GetInstance()->KeyboardCallback((int)wParam, true);
            break;
        case WM_KEYUP:
            WindowsApplication::GetInstance()->KeyboardCallback((int)wParam, false);
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

    void WindowsApplication::InitializePresentation()
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

		if (mHWnd != 0)
		{
			mHDC = GetDC(mHWnd);

			PIXELFORMATDESCRIPTOR pixelFormatDescr =
				{
					sizeof(PIXELFORMATDESCRIPTOR),
					1,
					PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
					PFD_TYPE_RGBA,
					32,
					0, 0, 0, 0, 0, 0, // color bits (ignored)
					0,				  // no alpha buffer
					0,				  // alpha bits (ignored)
					0,				  // no accumulation buffer
					0, 0, 0, 0,		  // accum bits (ignored)
					32,				  // depth buffer
					0,				  // no stencil buffer
					0,				  // no auxiliary buffers
					PFD_MAIN_PLANE,	  // main layer
					0,				  // reserved
					0, 0, 0,		  // no layer, visible, damage masks
				};
			auto pixelFormat = ChoosePixelFormat(mHDC, &pixelFormatDescr);
			if (!SetPixelFormat(mHDC, pixelFormat, &pixelFormatDescr))
			{
				FASTCG_THROW_EXCEPTION(Exception, "Error setting pixel format");
			}

			ShowWindow(mHWnd, SW_SHOW);
			UpdateWindow(mHWnd);
		}
		else
		{
			FASTCG_THROW_EXCEPTION(Exception, "Error creating window");
		}
    }

    void WindowsApplication::FinalizePresentation()
    {
        ReleaseDC(mHWnd, mHDC);
    }

    void WindowsApplication::Present()
    {
		SwapBuffers(mHDC);
    }
}

#endif