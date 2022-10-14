#ifndef FASTCG_BASE_APPLICATION_H
#define FASTCG_BASE_APPLICATION_H

#include <FastCG/Timer.h>
#include <FastCG/Shader.h>
#include <FastCG/RenderingStatistics.h>
#include <FastCG/Renderer.h>
#include <FastCG/RenderBatchingStrategy.h>
#include <FastCG/PointsRenderer.h>
#include <FastCG/PointLight.h>
#include <FastCG/MouseButton.h>
#include <FastCG/MeshFilter.h>
#include <FastCG/LineRenderer.h>
#include <FastCG/Light.h>
#include <FastCG/GameObject.h>
#include <FastCG/Font.h>
#include <FastCG/FastCG.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <cstdint>

namespace FastCG
{
	class ModelImporter;
	class InputSystem;
	class BaseRenderingSystem;

	enum class RenderingPath : uint8_t
	{
		RP_FORWARD_RENDERING = 0,
		RP_DEFERRED_RENDERING,

	};

	struct ApplicationSettings
	{
		std::string windowTitle;
		uint32_t screenWidth{1024};
		uint32_t screenHeight{768};
		uint32_t frameRate{60};
		RenderingPath renderingPath{RenderingPath::RP_FORWARD_RENDERING};
		std::string assetsPath{"../assets"};
		glm::vec4 clearColor{Colors::BLACK};
		glm::vec4 ambientLight{Colors::BLACK};
		bool showFPS{false};
		bool showRenderingStatistics{false};
	};

	class BaseApplication
	{
	public:
		BaseApplication(const ApplicationSettings &settings);
		virtual ~BaseApplication();

		inline static BaseApplication *GetInstance()
		{
			return s_mpInstance;
		}

		inline const std::string &GetWindowTitle() const
		{
			return mWindowTitle;
		}

		inline uint32_t GetScreenWidth() const
		{
			return mScreenWidth;
		}

		inline uint32_t GetScreenHeight() const
		{
			return mScreenHeight;
		}

		inline float GetAspectRatio() const
		{
			return mScreenWidth / (float)mScreenHeight;
		}

		inline Camera *GetMainCamera()
		{
			return mpMainCamera;
		}

		void SetMainCamera(Camera *pCamera);

		inline const glm::vec4 &GetClearColor() const
		{
			return mClearColor;
		}

		inline void SetClearColor(const glm::vec4 &clearColor)
		{
			mClearColor = clearColor;
		}

		int Run(int argc, char **argv);
		inline void Exit()
		{
			mRunning = false;
		}
		void BeforeMeshFilterChange(MeshFilter *pMeshFilter);
		void AfterMeshFilterChange(MeshFilter *pMeshFilter);

		friend class GameObject;

	protected:
		const ApplicationSettings mSettings;
		glm::vec4 mClearColor;
		glm::vec4 mAmbientLight;
		bool mShowFPS;
		bool mShowRenderingStatistics;
		bool mRunning{false};

		virtual bool ParseCommandLineArguments(int argc, char **argv);
		virtual void OnRegisterComponent() {}
		virtual void OnResize() {}
		virtual void OnStart() {}
		virtual void OnEnd() {}
		virtual void OnMouseButton(MouseButton button, MouseButtonState state, int x, int y) {}
		virtual void OnMouseWheel(int direction, int x, int y) {}
		virtual void OnMouseMove(int x, int y) {}
		virtual void OnKeyPress(int keyCode) {}
		virtual void OnKeyRelease(int keyCode) {}
		virtual void OnPrintUsage() {}
		virtual void RunMainLoop() = 0;
		virtual void InitializePresentation() = 0;
		virtual void FinalizePresentation() = 0;
		virtual void Present() = 0;
		inline void WindowResizeCallback(int width, int height)
		{
			mScreenWidth = width;
			mScreenHeight = height;
			if (mpMainCamera != nullptr)
			{
				mpMainCamera->SetAspectRatio(GetAspectRatio());
			}
			OnResize();
		}
		void MouseButtonCallback(MouseButton button, MouseButtonState state, int x, int y);
		void MouseWheelCallback(int direction, int x, int y);
		void MouseMoveCallback(int x, int y);
		void KeyboardCallback(int keyCode, bool pressed);
		inline void RunMainLoopIteration()
		{
			Update();
		}

	private:
		typedef void (*DeleteInputSystemCallback)(InputSystem *);
		typedef void (*DeleteBaseRenderingSystemCallback)(BaseRenderingSystem *);

		static BaseApplication *s_mpInstance;

		std::string mWindowTitle;
		uint32_t mScreenWidth;
		uint32_t mScreenHeight;
		std::string mAssetsPath;
		double mSecondsPerFrame;
		std::unique_ptr<InputSystem, DeleteInputSystemCallback> mpInputSystem{nullptr, nullptr};
		std::unique_ptr<BaseRenderingSystem, DeleteBaseRenderingSystemCallback> mpBaseRenderingSystem{nullptr, nullptr};
		std::unique_ptr<RenderBatchingStrategy> mpRenderBatchingStrategy{nullptr};
		Timer mStartTimer;
		Timer mFrameRateTimer;
		uint32_t mElapsedFrames{0};
		double mTotalElapsedTime{0};
		double mLastFrameTime{0};
		RenderingStatistics mRenderingStatistics;
		GameObject *mpInternalGameObject{nullptr};
		Camera *mpMainCamera{nullptr};
		std::vector<GameObject *> mGameObjects;
		std::vector<Camera *> mCameras;
		std::vector<DirectionalLight *> mDirectionalLights;
		std::vector<PointLight *> mPointLights;
		std::vector<MeshFilter *> mMeshFilters;
		std::vector<Behaviour *> mBehaviours;
		std::vector<LineRenderer *> mLineRenderers;
		std::vector<PointsRenderer *> mPointsRenderers;
		std::vector<Component *> mComponents;
		std::vector<std::unique_ptr<RenderBatch>> mRenderBatches;

		void RegisterGameObject(GameObject *pGameObject);
		void UnregisterGameObject(GameObject *pGameObject);
		void RegisterComponent(Component *pComponent);
		void RegisterCamera(Camera *pCamera);
		void UnregisterComponent(Component *pComponent);
		void DrawAllTexts();
		void ShowFPS();
		void ShowRenderingStatistics();
		void Update();
		void Render();
	};

}

#endif
