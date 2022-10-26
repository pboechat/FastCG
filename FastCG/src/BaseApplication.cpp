#include <FastCG/Thread.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/ShaderLoader.h>
#include <FastCG/Renderable.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/PointLight.h>
#include <FastCG/MouseButton.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MaterialBasedRenderBatchStrategy.h>
#include <FastCG/Light.h>
#include <FastCG/Key.h>
#include <FastCG/InputSystem.h>
#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>
#include <FastCG/BaseApplication.h>
#include <FastCG/AssetSystem.h>

#include <imgui.h>

#include <cstdio>
#include <cassert>
#include <algorithm>

namespace FastCG
{
	const std::string SHADERS_FOLDER = "shaders";
	const std::string FORWARD_RENDERING_SHADERS_FOLDER = SHADERS_FOLDER + "/forward";
	const std::string DEFERRED_RENDERING_SHADERS_FOLDER = SHADERS_FOLDER + "/deferred";
	const std::string FONTS_FOLDER = "fonts";

	BaseApplication *BaseApplication::smpInstance = nullptr;

#define FASTCG_REGISTER_COMPONENT(className, component)                    \
	if (component->GetType().IsDerived(className::TYPE))                   \
	{                                                                      \
		m##className##s.emplace_back(static_cast<className *>(component)); \
	}

#define FASTCG_UNREGISTER_COMPONENT(className, component)                               \
	if (component->GetType().IsDerived(className::TYPE))                                \
	{                                                                                   \
		auto it = std::find(m##className##s.begin(), m##className##s.end(), component); \
		if (it == m##className##s.end())                                                \
		{                                                                               \
			FASTCG_THROW_EXCEPTION(Exception, "Error unregistering: %s", #className);   \
		}                                                                               \
		m##className##s.erase(it);                                                      \
	}

#define FASTCG_IMPLEMENT_SYSTEM(className, argsClassName) \
	static className *sp##className = nullptr;            \
	void className::Create(const argsClassName &rArgs)    \
	{                                                     \
		sp##className = new className(rArgs);             \
	}                                                     \
	void className::Destroy()                             \
	{                                                     \
		delete sp##className;                             \
	}                                                     \
	className *className::GetInstance()                   \
	{                                                     \
		return sp##className;                             \
	}

	FASTCG_IMPLEMENT_SYSTEM(AssetSystem, AssetSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(InputSystem, InputSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(RenderingSystem, RenderingSystemArgs);

	BaseApplication::BaseApplication(const ApplicationSettings &settings) : mSettings(settings),
																			mWindowTitle(settings.windowTitle),
																			mScreenWidth(settings.screenWidth),
																			mScreenHeight(settings.screenHeight),
																			mClearColor(settings.clearColor),
																			mAmbientLight(settings.ambientLight),
																			mFrameRate(settings.frameRate),
																			mSecondsPerFrame(1 / (double)settings.frameRate)
	{
		if (smpInstance != nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "There can only be one BaseApplication instance");
		}

		smpInstance = this;
	}

	BaseApplication::~BaseApplication()
	{
		auto gameObjectsToDestroy = mGameObjects;
		for (auto *pGameObject : gameObjectsToDestroy)
		{
			GameObject::Destroy(pGameObject);
		}

		mpMainCamera = nullptr;

		assert(mGameObjects.empty());
		assert(mCameras.empty());
		assert(mDirectionalLights.empty());
		assert(mPointLights.empty());
		assert(mRenderables.empty());
		assert(mBehaviours.empty());
		assert(mComponents.empty());

		smpInstance = nullptr;
	}

	void BaseApplication::RegisterGameObject(GameObject *pGameObject)
	{
		assert(pGameObject != nullptr);
		mGameObjects.emplace_back(pGameObject);
	}

	void BaseApplication::UnregisterGameObject(GameObject *pGameObject)
	{
		assert(pGameObject != nullptr);
		auto it = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);
		assert(it != mGameObjects.end());
		mGameObjects.erase(it);
	}

	void BaseApplication::RegisterComponent(Component *pComponent)
	{
		assert(pComponent != nullptr);

		FASTCG_REGISTER_COMPONENT(DirectionalLight, pComponent);
		FASTCG_REGISTER_COMPONENT(PointLight, pComponent);
		FASTCG_REGISTER_COMPONENT(Renderable, pComponent);
		FASTCG_REGISTER_COMPONENT(Behaviour, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			RegisterCamera(static_cast<Camera *>(pComponent));
		}
		else if (pComponent->GetType().IsExactly(Renderable::TYPE))
		{
			mpRenderBatchStrategy->AddRenderable(static_cast<Renderable *>(pComponent));
		}

		mComponents.emplace_back(pComponent);
	}

	void BaseApplication::RegisterCamera(Camera *pCamera)
	{
		mCameras.emplace_back(pCamera);
		SetMainCamera(pCamera);
	}

	void BaseApplication::UnregisterComponent(Component *pComponent)
	{
		assert(pComponent != nullptr);

		FASTCG_UNREGISTER_COMPONENT(Camera, pComponent);
		FASTCG_UNREGISTER_COMPONENT(DirectionalLight, pComponent);
		FASTCG_UNREGISTER_COMPONENT(PointLight, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Renderable, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Behaviour, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Component, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			if (mCameras.size() > 1)
			{
				SetMainCamera(mCameras[0]);
			}
		}
		else if (pComponent->GetType().IsExactly(Renderable::TYPE))
		{
			mpRenderBatchStrategy->RemoveRenderable(static_cast<Renderable *>(pComponent));
		}
	}

	void BaseApplication::SetMainCamera(Camera *pCamera)
	{
		pCamera->SetEnabled(true);

		for (auto *pOtherCamera : mCameras)
		{
			if (pOtherCamera == pCamera)
			{
				continue;
			}

			pOtherCamera->SetEnabled(false);
		}

		mpMainCamera = pCamera;
		mpMainCamera->SetAspectRatio(GetAspectRatio());
	}

	int BaseApplication::Run(int argc, char **argv)
	{
		if (!ParseCommandLineArguments(argc, argv))
		{
			OnPrintUsage();
			return -1;
		}

		try
		{
			Initialize();

			mStartTimer.Start();
			OnStart();

			mRunning = true;
			RunMainLoop();

			mStartTimer.End();
			OnEnd();

			Finalize();

			return 0;
		}
		catch (Exception &e)
		{
			FASTCG_MSG_BOX("Error", "Fatal Exception: %s", e.GetFullDescription().c_str());
			return -1;
		}
	}

	void BaseApplication::Initialize()
	{
		ImGui::CreateContext();
		auto &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		AssetSystem::Create({mSettings.assetBundles});
		InputSystem::Create({});
		mpRenderBatchStrategy = std::make_unique<MaterialBasedRenderBatchStrategy>();
		RenderingSystem::Create({mSettings.renderingPath,
								 mScreenWidth,
								 mScreenHeight,
								 mClearColor,
								 mAmbientLight,
								 mDirectionalLights,
								 mPointLights,
								 mpRenderBatchStrategy->GetRenderBatches(),
								 mRenderingStatistics});

		RenderingSystem::GetInstance()->Initialize();

		int width, height;
		unsigned char *pixels = nullptr;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		auto *pImGuiTexture = RenderingSystem::GetInstance()->CreateTexture({"ImGui",
																			 (uint32_t)width,
																			 (uint32_t)height,
																			 TextureType::TEXTURE_2D,
																			 TextureFormat::RGBA,
																			 {32, 32, 32, 32},
																			 TextureDataType::UNSIGNED_CHAR,
																			 TextureFilter::LINEAR_FILTER,
																			 TextureWrapMode::CLAMP,
																			 false,
																			 pixels});
		io.Fonts->SetTexID((void *)pImGuiTexture);

		ShaderLoader::LoadShaders(mSettings.renderingPath);

		RenderingSystem::GetInstance()->PostInitialize();

		mpInternalGameObject = GameObject::Instantiate();
		Camera::Instantiate(mpInternalGameObject);

		OnInitialize();
	}

	void BaseApplication::Finalize()
	{
		OnFinalize();

		RenderingSystem::GetInstance()->Finalize();

		RenderingSystem::Destroy();
		InputSystem::Destroy();
		AssetSystem::Destroy();

		ImGui::DestroyContext();
	}

	bool BaseApplication::ParseCommandLineArguments(int argc, char **argv)
	{
		return true;
	}

	void BaseApplication::RunMainLoopIteration()
	{
		auto startTime = mStartTimer.GetTime();
		double deltaTime;
		if (mLastFrameTime != 0)
		{
			deltaTime = startTime - mLastFrameTime;
		}
		else
		{
			deltaTime = 0;
		}

		InputSystem::GetInstance()->Swap();

		auto &io = ImGui::GetIO();
		io.DisplaySize.x = (float)mScreenWidth;
		io.DisplaySize.y = (float)mScreenHeight;
		io.DeltaTime = (float)deltaTime + 0.0000001f;
		auto mousePos = InputSystem::GetMousePosition();
		io.AddMousePosEvent((float)mousePos.x, (float)mousePos.y);
		io.AddMouseButtonEvent(0, InputSystem::GetMouseButton((MouseButton)0) == MouseButtonState::PRESSED);
		io.AddMouseButtonEvent(1, InputSystem::GetMouseButton((MouseButton)1) == MouseButtonState::PRESSED);
		io.AddMouseButtonEvent(2, InputSystem::GetMouseButton((MouseButton)2) == MouseButtonState::PRESSED);

		ImGui::NewFrame();

		for (auto *pGameObject : mGameObjects)
		{
			if (pGameObject->GetTransform()->GetParent() != nullptr)
			{
				continue;
			}
			pGameObject->GetTransform()->Update();
		}

		for (auto *pBehaviour : mBehaviours)
		{
			pBehaviour->Update((float)startTime, (float)deltaTime);
		}

		RenderingSystem::GetInstance()->Render(mpMainCamera);

		ImGui::Begin("Statistics");
		ImGui::Text("FPS: %.3f", 1.0f / deltaTime);
		ImGui::Text("Draw Calls: %zu", mRenderingStatistics.drawCalls);
		ImGui::Text("Triangles: %zu", mRenderingStatistics.numberOfTriangles);
		ImGui::End();

		ImGui::EndFrame();
		ImGui::Render();

		RenderingSystem::GetInstance()->RenderImGui(ImGui::GetDrawData());

		mLastFrameTime = startTime;

		mTotalElapsedTime += deltaTime;
		mFrameCount++;

		RenderingSystem::GetInstance()->Present();

		if (mFrameRate != UNLOCKED_FRAMERATE)
		{
			auto idleTime = mSecondsPerFrame - deltaTime;
			if (idleTime > 0)
			{
				Thread::Sleep(idleTime);
				deltaTime += idleTime;
			}
		}
	}

	void BaseApplication::MouseButtonCallback(MouseButton button, MouseButtonState state)
	{
		InputSystem::GetInstance()->SetMouseButton(button, state);
		OnMouseButton(button, state);
	}

	void BaseApplication::MouseMoveCallback(uint32_t x, uint32_t y)
	{
		InputSystem::GetInstance()->SetMousePosition(glm::uvec2(x, y));
		OnMouseMove(x, y);
	}

	void BaseApplication::KeyboardCallback(Key key, bool pressed)
	{
		InputSystem::GetInstance()->SetKey(key, pressed);
		if (pressed)
		{
			OnKeyPress(key);
		}
		else
		{
			OnKeyRelease(key);
		}
	}

	void BaseApplication::WindowResizeCallback(uint32_t width, uint32_t height)
	{
		if (mScreenWidth == width && mScreenHeight == height)
		{
			return;
		}

		mScreenWidth = width;
		mScreenHeight = height;
		if (mpMainCamera != nullptr)
		{
			mpMainCamera->SetAspectRatio(GetAspectRatio());
		}
		RenderingSystem::GetInstance()->Resize();
		OnResize();
	}
}