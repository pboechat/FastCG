#include <Application.h>
#include <Behaviour.h>
#include <ShaderRegistry.h>
#include <FontRegistry.h>
#include <TextureImporter.h>
#include <ModelImporter.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <RenderingStrategy.h>
#include <ForwardRenderingStrategy.h>
#include <DeferredRenderingStrategy.h>
#include <MaterialGroupsBatchingStrategy.h>
#include <Colors.h>
#include <Thread.h>
#include <Exception.h>
#include <OpenGLExceptions.h>

#include <GL3/gl3w.h>

#include <algorithm>
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string Application::DEFERRED_RENDERING_SHADERS_FOLDER = "shaders/deferred";
const std::string Application::SHADERS_FOLDER = "shaders";
const std::string Application::FONTS_FOLDER = "fonts";
const std::string Application::DEFAULT_FONT_NAME = "verdana";
const std::string Application::WINDOW_CLASS_NAME = "windowClass";
const unsigned int Application::BYTES_PER_PIXEL = 4;
const unsigned int Application::COLOR_BUFFER_BITS = 32;
const unsigned int Application::DEPTH_BUFFER_BITS = 32;
const unsigned int Application::HAS_ALPHA = 0;
const PIXELFORMATDESCRIPTOR Application::PIXEL_FORMAT_DESCRIPTOR = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, Application::COLOR_BUFFER_BITS, 0, 0, 0, 0, 0, 0, Application::HAS_ALPHA, 0, 0, 0, 0, 0, 0, Application::DEPTH_BUFFER_BITS, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

Application* Application::s_mpInstance = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
#define WIN32_ASSERT(resultHandle, errorMessage) \
	if (resultHandle == 0) \
	{ \
		MessageBox(NULL, #errorMessage, mWindowTitle.c_str(), MB_OK | MB_ICONEXCLAMATION); \
		Dispose(); \
		exit(EXIT_FAILURE); \
	}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define REGISTER_COMPONENT(className, component) \
	if (component->GetType().IsDerived(className::TYPE)) \
	{ \
		m##className##s.push_back(dynamic_cast<className*>(component)); \
	} \

////////////////////////////////////////////////////////////////////////////////////////////////////
#define UNREGISTER_COMPONENT(className, component) \
	if (component->GetType().IsDerived(className::TYPE)) \
	{ \
		std::vector<className##*>::iterator it = std::find(m##className##s.begin(), m##className##s.end(), component); \
		if (it == m##className##s.end()) \
		{ \
			THROW_EXCEPTION(Exception, "Error unregistering: %s", #className); \
		} \
		m##className##s.erase(it); \
	} \

////////////////////////////////////////////////////////////////////////////////////////////////////
#define BEGIN_DRAW_TEXT() \
	glDisable(GL_DEPTH_TEST); \
	glEnable(GL_BLEND); \
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	
////////////////////////////////////////////////////////////////////////////////////////////////////
#define DRAW_TEXT(text, x, y, color) \
	mpStandardFont->DrawString(text, FontRegistry::STANDARD_FONT_SIZE, x, y, color)
	
////////////////////////////////////////////////////////////////////////////////////////////////////
#define END_DRAW_TEXT() \
	glDisable(GL_BLEND); \
	glEnable(GL_DEPTH_TEST)

////////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application(const std::string& rWindowTitle, unsigned int screenWidth, unsigned int screenHeight, unsigned int frameRate, bool deferredRendering, const std::string& rGlobalResourcePath) :
	mWindowTitle(rWindowTitle),
	mScreenWidth(screenWidth),
	mScreenHeight(screenHeight),
	mFrameRate(frameRate),
	mDeferredRendering(deferredRendering),
	mGlobalResourcePath(rGlobalResourcePath),
	mSecondsPerFrame(1.0 / (double)mFrameRate),
	mHalfScreenWidth(screenWidth / 2.0f),
	mHalfScreenHeight(screenHeight / 2.0f),
	mAspectRatio(mScreenWidth / (float) mScreenHeight),
	mClearColor(0.0f, 0.0f, 0.0f, 0.0f),
	mGlobalAmbientLight(Colors::BLACK),
	mApplicationHandle(0),
	mWindowHandle(0),
	mDeviceContextHandle(0),
	mPixelFormat(0),
	mOpenGLRenderingContextHandle(0),
	mShowFPS(false),
	mShowRenderingStatistics(false),
	mElapsedFrames(0),
	mTotalElapsedTime(0),
	mpInput(0),
	mpModelImporter(0),
	mpRenderingStrategy(0),
	mpRenderBatchingStrategy(0),
	mpStandardFont(0)
{
	s_mpInstance = this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
	if (mpRenderingStrategy != 0)
	{
		delete mpRenderingStrategy;
	}

	if (mpInput != 0)
	{
		delete mpInput;
	}

	mpMainCamera = 0;

	for (unsigned int i = 0; i < mDrawTextRequests.size(); i++)
	{
		delete mDrawTextRequests[i];
	}
	mDrawTextRequests.clear();

	ShaderRegistry::Unload();
	FontRegistry::Unload();
	mpStandardFont = 0;

	std::vector<GameObject*> gameObjectsToDestroy = mGameObjects;
	for (unsigned int i = 0; i < gameObjectsToDestroy.size(); i++)
	{
		GameObject::Destroy(gameObjectsToDestroy[i]);
	}
	gameObjectsToDestroy.clear();

	if (mGameObjects.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mGameObjects.size() > 0");
	}

	if (mCameras.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mCameras.size() > 0");
	}

	if (mLights.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mLights.size() > 0");
	}

	if (mDirectionalLights.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mDirectionalLights.size() > 0");
	}

	if (mPointLights.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mPointLights.size() > 0");
	}

	if (mMeshFilters.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mMeshFilters.size() > 0");
	}

	if (mBehaviours.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mBehaviours.size() > 0");
	}

	if (mLineRenderers.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mLineRenderers.size() > 0");
	}

	if (mPointsRenderers.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mPointsRenderers.size() > 0");
	}

	if (mComponents.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mComponents.size() > 0");
	}

	if (mpRenderBatchingStrategy != 0)
	{
		delete mpRenderBatchingStrategy;
	}

	for (unsigned int i = 0; i < mRenderBatches.size(); i++)
	{
		delete mRenderBatches[i];
	}
	mRenderBatches.clear();

	ModelImporter::Dispose();
	TextureImporter::Dispose();

	s_mpInstance = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::RegisterGameObject(GameObject* pGameObject)
{
	if (pGameObject == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pGameObject == 0");
	}

	mGameObjects.push_back(pGameObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::UnregisterGameObject(GameObject* pGameObject)
{
	if (pGameObject == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pGameObject == 0");
	}

	std::vector<GameObject*>::iterator gameObjectsIterator = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);

	if (gameObjectsIterator == mGameObjects.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "gameObjectsIterator == mGameObjects.end()");
	}

	mGameObjects.erase(gameObjectsIterator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::RegisterComponent(Component* pComponent)
{
	if (pComponent == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pComponent == 0");
	}

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

	mComponents.push_back(pComponent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::RegisterCamera(Camera* pCamera)
{
	mCameras.push_back(pCamera);
	SetMainCamera(pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::UnregisterComponent(Component* pComponent)
{
	if (pComponent == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pComponent == 0");
	}

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

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SetMainCamera(Camera* pCamera)
{
	pCamera->SetEnabled(true);

	for (unsigned int i = 0; i < mCameras.size(); i++)
	{
		Camera* pOtherCamera = mCameras[i];

		if (pOtherCamera == pCamera)
		{
			continue;
		}

		pOtherCamera->SetEnabled(false);
	}

	mpMainCamera = pCamera;
	mpMainCamera->SetAspectRatio(mAspectRatio);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int Application::Run(int argc, char** argv)
{
	mApplicationTimer.Start();

	if (!ParseCommandLineArguments(argc, argv))
	{
		PrintUsage();
		return EXIT_FAILURE;
	}

	mApplicationHandle = GetModuleHandle(0);

	WIN32_ASSERT(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	WIN32_ASSERT((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME.c_str(), mWindowTitle.c_str(), (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN), CW_USEDEFAULT, CW_USEDEFAULT, mScreenWidth, mScreenHeight, 0, 0, mApplicationHandle, 0)), "CreateWindow failed");
	WIN32_ASSERT((mDeviceContextHandle = GetDC(mWindowHandle)), "GetDC() failed");
	WIN32_ASSERT((mPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &PIXEL_FORMAT_DESCRIPTOR)), "ChoosePixelFormat() failed");
	WIN32_ASSERT(SetPixelFormat(mDeviceContextHandle, mPixelFormat, &PIXEL_FORMAT_DESCRIPTOR), "SetPixelFormat() failed");
	WIN32_ASSERT((mOpenGLRenderingContextHandle = wglCreateContext(mDeviceContextHandle)), "wglCreateContext() failed");
	WIN32_ASSERT(wglMakeCurrent(mDeviceContextHandle, mOpenGLRenderingContextHandle), "wglMakeCurrent() failed");

	ShowWindow(mWindowHandle, SW_SHOW);
	SetForegroundWindow(mWindowHandle);
	SetFocus(mWindowHandle);

	int returnCode = -1;
	if (gl3wInit())
	{
		MessageBox(mWindowHandle, "Couldn't initialize gl3w", mWindowTitle.c_str(), MB_OK | MB_ICONERROR);
		DestroyWindow(mWindowHandle);
		returnCode = EXIT_FAILURE;
	}
	else
	{
		try
		{
			std::string shadersFolder;

			ShaderRegistry::LoadShadersFromDisk(mGlobalResourcePath + ((mDeferredRendering) ? DEFERRED_RENDERING_SHADERS_FOLDER : SHADERS_FOLDER));
			FontRegistry::LoadFontsFromDisk(mGlobalResourcePath + FONTS_FOLDER);
			mpStandardFont = FontRegistry::Find(DEFAULT_FONT_NAME);

			if (mDeferredRendering)
			{
				mpRenderingStrategy = new DeferredRenderingStrategy(mLights, mDirectionalLights, mPointLights, mGlobalAmbientLight, mRenderBatches, mLineRenderers, mPointsRenderers, mRenderingStatistics, mScreenWidth, mScreenHeight);
			}
			else
			{
				mpRenderingStrategy = new ForwardRenderingStrategy(mLights, mDirectionalLights, mPointLights, mGlobalAmbientLight, mRenderBatches, mLineRenderers, mPointsRenderers, mRenderingStatistics);
			}
			mpRenderBatchingStrategy = new MaterialGroupsBatchingStrategy(mRenderBatches);

			TextureImporter::Initialize();
			ModelImporter::Initialize();

			mpInput = new Input();
			mpInternalGameObject = GameObject::Instantiate();
			Camera* pCamera = Camera::Instantiate(mpInternalGameObject);

			OnStart();

			bool running = true;
			MSG message;
			while (running)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
					{
						running = false;
					}
					else
					{
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
				}
				else
				{
					Update();
				}
			}

			returnCode = (int) message.wParam;
		}
		catch (Exception& e)
		{
			MessageBox(mWindowHandle, e.GetFullDescription().c_str(), mWindowTitle.c_str(), MB_OK | MB_ICONERROR);
			DestroyWindow(mWindowHandle);
			returnCode = EXIT_FAILURE;
		}
	}

	Dispose();

	WIN32_ASSERT(UnregisterClass(WINDOW_CLASS_NAME.c_str(), mApplicationHandle), "UnregisterClass() failed");

	mDeviceContextHandle = 0;
	mWindowHandle = 0;
	mApplicationHandle = 0;

	OnEnd();

	mApplicationTimer.End();

	return returnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Application::ParseCommandLineArguments(int argc, char** argv)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SetUpOpenGL()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Update()
{
	static double deltaTime = 0.0;

	double start = mApplicationTimer.GetTime();

	mpInput->Swap();

	for (unsigned int i = 0; i < mGameObjects.size(); i++)
	{
		if (mGameObjects[i]->GetTransform()->GetParent() != 0)
		{
			continue;
		}
		mGameObjects[i]->GetTransform()->Update();
	}

	for (unsigned int i = 0; i < mBehaviours.size(); i++)
	{
		mBehaviours[i]->Update((float)mApplicationTimer.GetTime(), (float)deltaTime);
	}

	Render();

	double end = mApplicationTimer.GetTime();

	deltaTime = end - start;

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

	SwapBuffers(mDeviceContextHandle);

	/*double idleTime = mSecondsPerFrame - deltaTime; // frame rate cycles/second
	if (idleTime > 0)
	{
		Thread::Sleep(idleTime);
		deltaTime += idleTime;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::DrawAllTexts()
{
	BEGIN_DRAW_TEXT();

	for (unsigned int i = 0; i < mDrawTextRequests.size(); i++)
	{
		DrawTextRequest* pDrawTextRequest = mDrawTextRequests[i];
		DRAW_TEXT(pDrawTextRequest->text, pDrawTextRequest->x, (mScreenHeight - pDrawTextRequest->y), pDrawTextRequest->color);
		delete pDrawTextRequest;
	}

	mDrawTextRequests.clear();

	END_DRAW_TEXT();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::ShowFPS()
{
	static char fpsText[128];

	BEGIN_DRAW_TEXT();

	sprintf(fpsText, "FPS: %.3f", mElapsedFrames / mTotalElapsedTime);
	DRAW_TEXT(fpsText, mScreenWidth - 240, (mScreenHeight - 17), Colors::GREEN);

	END_DRAW_TEXT();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::ShowRenderingStatistics()
{
	static char text[128];

	BEGIN_DRAW_TEXT();

	sprintf(text, "Draw Calls: %d", mRenderingStatistics.drawCalls);
	DRAW_TEXT(text, mScreenWidth - 240, (mScreenHeight - 34), Colors::GREEN);

	sprintf(text, "No. Triangles: %d", mRenderingStatistics.numberOfTriangles);
	DRAW_TEXT(text, mScreenWidth - 240, (mScreenHeight - 51), Colors::GREEN);

	END_DRAW_TEXT();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Render()
{
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);
	mpRenderingStrategy->Render(mpMainCamera);
	DrawAllTexts();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, Font* pFont, const glm::vec4& rColor)
{
	mDrawTextRequests.push_back(new DrawTextRequest(rText, size, x, y, pFont, rColor));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
{
	mDrawTextRequests.push_back(new DrawTextRequest(rText, size, x, y, mpStandardFont, rColor));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::BeforeMeshFilterChange(MeshFilter* pMeshFilter)
{
	mpRenderBatchingStrategy->RemoveMeshFilter(pMeshFilter);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::AfterMeshFilterChange(MeshFilter* pMeshFilter)
{
	mpRenderBatchingStrategy->AddMeshFilter(pMeshFilter);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Resize(int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	mHalfScreenWidth = width * 0.5f;
	mHalfScreenHeight = height * 0.5f;
	mAspectRatio = mScreenWidth / (float) mScreenHeight;
	mpMainCamera->SetAspectRatio(mAspectRatio);
	SetUpViewport();
	OnResize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SetUpViewport()
{
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Dispose()
{
	if (mOpenGLRenderingContextHandle)
	{
		WIN32_ASSERT(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
		WIN32_ASSERT(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
		mOpenGLRenderingContextHandle = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
WNDCLASSEX Application::CreateWindowClass()
{
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProcedure;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = mApplicationHandle;
	windowClass.hIcon = LoadIcon(mApplicationHandle, MAKEINTRESOURCE(IDI_APPLICATION));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = WINDOW_CLASS_NAME.c_str();
	windowClass.hIconSm	= LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	return windowClass;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Exit()
{
	DestroyWindow(mWindowHandle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::MouseButton(int button, int state, int x, int y)
{
	if (mpInput == 0)
	{
		return;
	}

	mpInput->SetMouseButton(button, (state == MouseButton::PRESSED));
	OnMouseButton(button, state, x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::MouseWheel(int button, int direction, int x, int y)
{
	if (mpInput == 0)
	{
		return;
	}

	if (direction == 1)
	{
		mpInput->IncrementMouseWheelDelta();
	}

	else if (direction == -1)
	{
		mpInput->DecrementMouseWheelDelta();
	}

	OnMouseWheel(button, direction, x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::MouseMove(int x, int y)
{
	if (mpInput == 0)
	{
		return;
	}

	mpInput->SetMousePosition(glm::vec2((float)x, (float)(mScreenHeight - y)));
	OnMouseMove(x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Keyboard(int keyCode, bool state)
{
	if (mpInput == 0)
	{
		return;
	}

	mpInput->SetKey(keyCode, state);

	if (state)
	{
		OnKeyPress(keyCode);
	}

	else
	{
		OnKeyRelease(keyCode);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::PrintUsage()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnStart()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnResize()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnEnd()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnMouseButton(int button, int state, int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnMouseWheel(int button, int direction, int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnMouseMove(int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnKeyPress(int keyCode)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::OnKeyRelease(int keyCode)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		Application::s_mpInstance->Keyboard(wParam, true);
		break;
	case WM_KEYUP:
		Application::s_mpInstance->Keyboard(wParam, false);
		break;
	case WM_LBUTTONDOWN:
		Application::s_mpInstance->MouseButton(MK_LBUTTON, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONDOWN:
		Application::s_mpInstance->MouseButton(MK_MBUTTON, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDOWN:
		Application::s_mpInstance->MouseButton(MK_RBUTTON, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		Application::s_mpInstance->MouseButton(MK_LBUTTON, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONUP:
		Application::s_mpInstance->MouseButton(MK_MBUTTON, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONUP:
		Application::s_mpInstance->MouseButton(MK_RBUTTON, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEMOVE:
		Application::s_mpInstance->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}