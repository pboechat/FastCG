#include <FastCG/Application.h>
#include <FastCG/Behaviour.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/Input.h>
#include <FastCG/KeyCode.h>
#include <FastCG/MouseButton.h>
#include <FastCG/RenderingStrategy.h>
#include <FastCG/ForwardRenderingStrategy.h>
#include <FastCG/DeferredRenderingStrategy.h>
#include <FastCG/MaterialGroupsBatchingStrategy.h>
#include <FastCG/Colors.h>
#include <FastCG/Thread.h>
#include <FastCG/Exception.h>
#include <FastCG/OpenGLExceptions.h>

#include <GL/wglew.h>
#include <GL/gl.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <cassert>
#include <algorithm>

namespace FastCG
{
	const std::string DEFERRED_RENDERING_SHADERS_FOLDER = "shaders/deferred";
	const std::string SHADERS_FOLDER = "shaders";
	const std::string FONTS_FOLDER = "fonts";
	const std::string DEFAULT_FONT_NAME = "verdana";

	Application* Application::s_mpInstance = nullptr;

#define REGISTER_COMPONENT(className, component) \
	if (component->GetType().IsDerived(className::TYPE)) \
	{ \
		m##className##s.emplace_back(dynamic_cast<className*>(component)); \
	}

#define UNREGISTER_COMPONENT(className, component) \
	if (component->GetType().IsDerived(className::TYPE)) \
	{ \
		auto it = std::find(m##className##s.begin(), m##className##s.end(), component); \
		if (it == m##className##s.end()) \
		{ \
			THROW_EXCEPTION(Exception, "Error unregistering: %s", #className); \
		} \
		m##className##s.erase(it); \
	}

#define MSG_BOX(title, fmt, ...) \
	{ \
		char msg[4096]; \
		sprintf_s(msg, sizeof(msg) / sizeof(char), fmt, ##__VA_ARGS__); \
		MessageBoxA(NULL, msg, title, MB_ICONWARNING); \
	}

#define DRAW_TEXT(text, x, y, color) \
	mpStandardFont->DrawString(text, x, y, color)

#ifdef _WIN32
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
			Application::GetInstance()->WindowResizeCallback(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_LBUTTONDOWN:
			Application::GetInstance()->MouseButtonCallback(MouseButton::LEFT_BUTTON, MouseButtonState::PRESSED, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_RBUTTONDOWN:
			Application::GetInstance()->MouseButtonCallback(MouseButton::RIGHT_BUTTON, MouseButtonState::PRESSED, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MBUTTONDOWN:
			Application::GetInstance()->MouseButtonCallback(MouseButton::MIDDLE_BUTTON, MouseButtonState::PRESSED, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_LBUTTONUP:
			Application::GetInstance()->MouseButtonCallback(MouseButton::LEFT_BUTTON, MouseButtonState::RELEASED, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_RBUTTONUP:
			Application::GetInstance()->MouseButtonCallback(MouseButton::RIGHT_BUTTON, MouseButtonState::RELEASED, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MBUTTONUP:
			Application::GetInstance()->MouseButtonCallback(MouseButton::MIDDLE_BUTTON, MouseButtonState::RELEASED, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEMOVE:
			Application::GetInstance()->MouseMoveCallback(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_KEYDOWN:
			Application::GetInstance()->KeyboardCallback((int)wParam, true);
			break;
		case WM_KEYUP:
			Application::GetInstance()->KeyboardCallback((int)wParam, false);
			break;
		default:
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
#endif

	Application::Application(const std::string& rWindowTitle, uint32_t screenWidth, uint32_t screenHeight, uint32_t frameRate, bool deferredRendering, const std::string& rAssetsPath) :
		mWindowTitle(rWindowTitle),
		mScreenWidth(screenWidth),
		mScreenHeight(screenHeight),
		mFrameRate(frameRate),
		mDeferredRendering(deferredRendering),
		mAssetsPath(rAssetsPath),
		mSecondsPerFrame(1.0 / (double)mFrameRate),
		mHalfScreenWidth(screenWidth / 2.0f),
		mHalfScreenHeight(screenHeight / 2.0f),
		mAspectRatio(mScreenWidth / (float)mScreenHeight)
	{
		s_mpInstance = this;
	}

	Application::~Application()
	{
		if (mpRenderingStrategy != nullptr)
		{
			delete mpRenderingStrategy;
		}

		if (mpInput != nullptr)
		{
			delete mpInput;
		}

		mDrawTextRequests.clear();

		ShaderRegistry::Unload();
		FontRegistry::Unload();

		mpStandardFont = nullptr;

		auto gameObjectsToDestroy = mGameObjects;
		for (auto* pGameObject : gameObjectsToDestroy)
		{
			GameObject::Destroy(pGameObject);
		}

		mpMainCamera = nullptr;

		assert(mGameObjects.empty());
		assert(mCameras.empty());
		assert(mLights.empty());
		assert(mDirectionalLights.empty());
		assert(mPointLights.empty());
		assert(mMeshFilters.empty());
		assert(mBehaviours.empty());
		assert(mLineRenderers.empty());
		assert(mPointsRenderers.empty());
		assert(mComponents.empty());

		mRenderBatches.clear();

		ModelImporter::Dispose();

		s_mpInstance = nullptr;
	}

	void Application::RegisterGameObject(GameObject* pGameObject)
	{
		assert(pGameObject != nullptr);
		mGameObjects.emplace_back(pGameObject);
	}

	void Application::UnregisterGameObject(GameObject* pGameObject)
	{
		assert(pGameObject != nullptr);
		auto it = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);
		assert(it != mGameObjects.end());
		mGameObjects.erase(it);
	}

	void Application::RegisterComponent(Component* pComponent)
	{
		assert(pComponent != nullptr);

		REGISTER_COMPONENT(Light, pComponent);
		REGISTER_COMPONENT(DirectionalLight, pComponent);
		REGISTER_COMPONENT(PointLight, pComponent);
		REGISTER_COMPONENT(MeshFilter, pComponent);
		REGISTER_COMPONENT(LineRenderer, pComponent);
		REGISTER_COMPONENT(PointsRenderer, pComponent);
		REGISTER_COMPONENT(Behaviour, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			RegisterCamera(dynamic_cast<Camera*>(pComponent));
		}
		else if (pComponent->GetType().IsExactly(MeshFilter::TYPE))
		{
			mpRenderBatchingStrategy->AddMeshFilter(dynamic_cast<MeshFilter*>(pComponent));
		}

		mComponents.emplace_back(pComponent);
	}

	void Application::RegisterCamera(Camera* pCamera)
	{
		mCameras.emplace_back(pCamera);
		SetMainCamera(pCamera);
	}

	void Application::UnregisterComponent(Component* pComponent)
	{
		assert(pComponent != nullptr);

		UNREGISTER_COMPONENT(Camera, pComponent);
		UNREGISTER_COMPONENT(Light, pComponent);
		UNREGISTER_COMPONENT(DirectionalLight, pComponent);
		UNREGISTER_COMPONENT(PointLight, pComponent);
		UNREGISTER_COMPONENT(LineRenderer, pComponent);
		UNREGISTER_COMPONENT(PointsRenderer, pComponent);
		UNREGISTER_COMPONENT(MeshFilter, pComponent);
		UNREGISTER_COMPONENT(Behaviour, pComponent);
		UNREGISTER_COMPONENT(Component, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			if (mCameras.size() > 1)
			{
				SetMainCamera(mCameras[0]);
			}
		}
		else if (pComponent->GetType().IsExactly(MeshFilter::TYPE))
		{
			mpRenderBatchingStrategy->RemoveMeshFilter(dynamic_cast<MeshFilter*>(pComponent));
		}
	}

	void Application::SetMainCamera(Camera* pCamera)
	{
		pCamera->SetEnabled(true);

		for (auto* pOtherCamera : mCameras)
		{
			if (pOtherCamera == pCamera)
			{
				continue;
			}

			pOtherCamera->SetEnabled(false);
		}

		mpMainCamera = pCamera;
		mpMainCamera->SetAspectRatio(mAspectRatio);
	}

	int Application::Run(int argc, char** argv)
	{
		if (!ParseCommandLineArguments(argc, argv))
		{
			OnPrintUsage();
			return -1;
		}

		try
		{
			mpInput = new Input();

			SetUpPresentation();
			SetUpOpenGL();

			ShaderRegistry::LoadShadersFromDisk(mAssetsPath + "/" + ((mDeferredRendering) ? DEFERRED_RENDERING_SHADERS_FOLDER : SHADERS_FOLDER));
			FontRegistry::LoadFontsFromDisk(mAssetsPath + "/" + FONTS_FOLDER);
			TextureImporter::SetBasePath(mAssetsPath);
			ModelImporter::SetBasePath(mAssetsPath);

			mpStandardFont = FontRegistry::Find(DEFAULT_FONT_NAME);

			if (mDeferredRendering)
			{
				mpRenderingStrategy = new DeferredRenderingStrategy(mScreenWidth, mScreenHeight, mLights, mDirectionalLights, mPointLights, mGlobalAmbientLight, mRenderBatches, mLineRenderers, mPointsRenderers, mRenderingStatistics);
			}
			else
			{
				mpRenderingStrategy = new ForwardRenderingStrategy(mScreenWidth, mScreenHeight, mLights, mDirectionalLights, mPointLights, mGlobalAmbientLight, mRenderBatches, mLineRenderers, mPointsRenderers, mRenderingStatistics);
			}

			mpRenderBatchingStrategy = std::make_unique<MaterialGroupsBatchingStrategy>(mRenderBatches);

			mpInternalGameObject = GameObject::Instantiate();

			auto* pCamera = Camera::Instantiate(mpInternalGameObject);

			mStartTimer.Start();
			OnStart();

			mRunning = true;
			RunMainLoop();

			mStartTimer.End();
			OnEnd();

			TearDownOpenGL();
			TearDownPresentation();

			return 0;
		}
		catch (Exception& e)
		{
			MSG_BOX("Error", "Fatal Exception: %s", e.GetFullDescription().c_str());
			return -1;
		}
	}

	void Application::TearDownOpenGL()
	{
		DestroyOpenGLContext();
	}

	void Application::DestroyOpenGLContext()
	{
#ifdef _WIN32
		if (mHGLRC != 0)
		{
			wglMakeCurrent(mHDC, NULL);
			wglDeleteContext(mHGLRC);
		}
#else
#error "FastCG::Application::DestroyOpenGLContext() is not implemented on the current platform"
#endif
	}

	void Application::TearDownPresentation()
	{
#ifdef _WIN32
		ReleaseDC(mHWnd, mHDC);
#else
#error "FastCG::Application::TearDownPresentation() is not implemented on the current platform"
#endif
	}

	void Application::RunMainLoop()
	{
#ifdef _WIN32
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

			Update();
		}
#else
#error "FastCG::Application::RunMainLoop() is not implement on the current platform"
#endif
	}

	bool Application::ParseCommandLineArguments(int argc, char** argv)
	{
		return true;
	}

	void Application::SetUpPresentation()
	{
#ifdef _WIN32
		mHInstance = GetModuleHandle(NULL);

		RECT windowRect;
		windowRect.left = (LONG)0;
		windowRect.right = (LONG)mScreenWidth;
		windowRect.top = (LONG)0;
		windowRect.bottom = (LONG)mScreenHeight;

		WNDCLASSEX windowClass;
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = mHInstance;
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// default icon
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// default arrow
		windowClass.hbrBackground = NULL;						// don't need background
		windowClass.lpszMenuName = NULL;						// no menu
		windowClass.lpszClassName = "FastCG_Window";
		windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);		// windows logo small icon

		if (!RegisterClassEx(&windowClass))
		{
			THROW_EXCEPTION(Exception, "Error registering window class");
		}

		auto dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		auto dwStyle = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
		mHWnd = CreateWindowEx(NULL,
			"FastCG_Window",
			mWindowTitle.c_str(),
			dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			mHInstance,
			NULL
		);

		if (mHWnd == 0)
		{
			THROW_EXCEPTION(Exception, "Error creating window");
		}

		mHDC = GetDC(mHWnd);

		PIXELFORMATDESCRIPTOR pixelFormatDescr =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,			// color bits (ignored)
			0,							// no alpha buffer
			0,							// alpha bits (ignored)
			0,							// no accumulation buffer
			0, 0, 0, 0,					// accum bits (ignored)
			32,							// depth buffer
			0,							// no stencil buffer
			0,							// no auxiliary buffers
			PFD_MAIN_PLANE,				// main layer
			0,							// reserved
			0, 0, 0,					// no layer, visible, damage masks
		};
		auto pixelFormat = ChoosePixelFormat(mHDC, &pixelFormatDescr);
		if (!SetPixelFormat(mHDC, pixelFormat, &pixelFormatDescr))
		{
			THROW_EXCEPTION(Exception, "Error setting pixel format");
		}

		ShowWindow(mHWnd, SW_SHOW);
		UpdateWindow(mHWnd);
#else
#error FastCG::Application::SetUpPresentation() is not implemented on the current platform
#endif
	}

	void Application::SetUpOpenGL()
	{
		CreateOpenGLContext();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);
	}

	void Application::CreateOpenGLContext()
	{
#ifdef _WIN32
		auto tmpHGLRC = wglCreateContext(mHDC);
		if (tmpHGLRC == 0)
		{
			THROW_EXCEPTION(Exception, "Error creating a temporary GL context");
		}

		if (!wglMakeCurrent(mHDC, tmpHGLRC))
		{
			THROW_EXCEPTION(Exception, "Error making the temporary GL context current");
		}

		{
			GLenum glewInitRes;
			if ((glewInitRes = glewInit()) != GLEW_OK)
			{
				THROW_EXCEPTION(Exception, "Error intializing glew: %s", glewGetErrorString(glewInitRes));
			}
		}

		const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};
		mHGLRC = wglCreateContextAttribsARB(mHDC, mHGLRC, attribs);
		if (mHGLRC == 0)
		{
			THROW_EXCEPTION(Exception, "Error creating the final GL context");
		}

		if (!wglMakeCurrent(mHDC, mHGLRC))
		{
			THROW_EXCEPTION(Exception, "Error making GL context current");
		}

		wglDeleteContext(tmpHGLRC);
#else
#error FastCG::Application::CreateOpenGLContext() is not implemented on the current platform
#endif
	}

	void Application::Update()
	{
		auto currTime = mStartTimer.GetTime();
		double deltaTime;
		if (mLastFrameTime != 0)
		{
			deltaTime = currTime - mLastFrameTime;
		}
		else
		{
			deltaTime = 0;
		}

		mpInput->Swap();

		for (auto* pGameObject : mGameObjects)
		{
			if (pGameObject->GetTransform()->GetParent() != 0)
			{
				continue;
			}
			pGameObject->GetTransform()->Update();
		}

		for (auto* pBehaviour : mBehaviours)
		{
			pBehaviour->Update((float)mStartTimer.GetTime(), (float)deltaTime);
		}

		Render();

		mLastFrameTime = currTime;

		mTotalElapsedTime += deltaTime;
		mElapsedFrames++;

		if (mShowFPS)
		{
			ShowFPS();
		}

		if (mShowRenderingStatistics)
		{
			ShowRenderingStatistics();
		}

		Present();

		auto idleTime = mSecondsPerFrame - deltaTime;
		if (idleTime > 0)
		{
			Thread::Sleep(idleTime);
			deltaTime += idleTime;
		}
	}

	void Application::Present()
	{
#ifdef _WIN32
		SwapBuffers(mHDC);
#else
#error "FastCG::Application::Present() is not implemented on the current platform"
#endif
	}

	void Application::DrawAllTexts()
	{
		glViewport(0, 0, mScreenWidth, mScreenHeight);

		for (auto& rDrawTextRequest : mDrawTextRequests)
		{
			DRAW_TEXT(rDrawTextRequest.text, rDrawTextRequest.x, (mScreenHeight - rDrawTextRequest.y), rDrawTextRequest.color);
		}
		mDrawTextRequests.clear();
	}

	void Application::ShowFPS()
	{
		char fpsText[128];

		sprintf_s(fpsText, sizeof(fpsText) / sizeof(char), "FPS: %.3f", mElapsedFrames / mTotalElapsedTime);
		DRAW_TEXT(fpsText, mScreenWidth - 240, (mScreenHeight - 17), Colors::GREEN);
	}

	void Application::ShowRenderingStatistics()
	{
		char text[128];

		sprintf_s(text, sizeof(text) / sizeof(char), "Draw Calls: %zu", mRenderingStatistics.drawCalls);
		DRAW_TEXT(text, mScreenWidth - 240, (mScreenHeight - 34), Colors::GREEN);

		sprintf_s(text, sizeof(text) / sizeof(char), "No. Triangles: %zu", mRenderingStatistics.numberOfTriangles);
		DRAW_TEXT(text, mScreenWidth - 240, (mScreenHeight - 51), Colors::GREEN);
	}

	void Application::Render()
	{
		if (mHGLRC == 0)
		{
			return;
		}

		glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);
		mpRenderingStrategy->Render(mpMainCamera);
		DrawAllTexts();
	}

	void Application::DrawText(const std::string& rText, uint32_t x, uint32_t y, Font* pFont, const glm::vec4& rColor)
	{
		mDrawTextRequests.emplace_back(DrawTextRequest{ rText, x, y, pFont, rColor });
	}

	void Application::DrawText(const std::string& rText, uint32_t x, uint32_t y, const glm::vec4& rColor)
	{
		mDrawTextRequests.emplace_back(DrawTextRequest{ rText, x, y, mpStandardFont, rColor });
	}

	void Application::BeforeMeshFilterChange(MeshFilter* pMeshFilter)
	{
		mpRenderBatchingStrategy->RemoveMeshFilter(pMeshFilter);
	}

	void Application::AfterMeshFilterChange(MeshFilter* pMeshFilter)
	{
		mpRenderBatchingStrategy->AddMeshFilter(pMeshFilter);
	}

	void Application::WindowResizeCallback(int width, int height)
	{
		mScreenWidth = width;
		mScreenHeight = height;
		mHalfScreenWidth = width * 0.5f;
		mHalfScreenHeight = height * 0.5f;
		mAspectRatio = mScreenWidth / (float)mScreenHeight;
		if (mpMainCamera != nullptr)
		{
			mpMainCamera->SetAspectRatio(mAspectRatio);
		}
		OnResize();
	}

	void Application::Exit()
	{
		mRunning = false;
	}

	void Application::MouseButtonCallback(MouseButton button, MouseButtonState state, int x, int y)
	{
		mpInput->SetMouseButton(button, state);
		OnMouseButton(button, state, x, y);
	}

	void Application::MouseWheelCallback(int direction, int x, int y)
	{
		if (direction == 1)
		{
			mpInput->IncrementMouseWheelDelta();
		}
		else if (direction == -1)
		{
			mpInput->DecrementMouseWheelDelta();
		}

		OnMouseWheel(direction, x, y);
	}

	void Application::MouseMoveCallback(int x, int y)
	{
		mpInput->SetMousePosition(glm::vec2((float)x, (float)(mScreenHeight - y)));
		OnMouseMove(x, y);
	}

	void Application::KeyboardCallback(int keyCode, bool pressed)
	{
		mpInput->SetKey(keyCode, pressed);
		if (pressed)
		{
			OnKeyPress(keyCode);
		}
		else
		{
			OnKeyRelease(keyCode);
		}
	}

}