#ifndef FASTCG_BASE_APPLICATION_H
#define FASTCG_BASE_APPLICATION_H

#include <FastCG/Timer.h>
#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderingPath.h>
#include <FastCG/MouseButton.h>
#include <FastCG/Key.h>
#include <FastCG/FastCG.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace FastCG
{
	class ModelImporter;
	class RenderBatchStrategy;
	class Renderable;
	class PointLight;
	class ImGuiRenderer;
	class IWorldRenderer;
	class GameObject;
	class DirectionalLight;
	class Component;
	class Camera;
	class Behaviour;

	constexpr uint32_t UNLOCKED_FRAMERATE = ~0u;

	struct ApplicationSettings
	{
		std::string windowTitle{};
		uint32_t screenWidth{1024};
		uint32_t screenHeight{768};
		uint32_t frameRate{60};
		RenderingPath renderingPath{RenderingPath::FORWARD_RENDERING};
		std::vector<std::string> assetBundles{};
		glm::vec4 clearColor{Colors::BLACK};
		glm::vec4 ambientLight{Colors::BLACK};
	};

	class BaseApplication
	{
	public:
		BaseApplication(const ApplicationSettings &settings);
		virtual ~BaseApplication();

		inline static BaseApplication *GetInstance()
		{
			return smpInstance;
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

		inline uint32_t GetFrameRate() const
		{
			return mFrameRate;
		}

		inline void SetFrameRate(uint32_t frameRate)
		{
			mFrameRate = frameRate;
			mSecondsPerFrame = 1 / (double)frameRate;
		}

		inline IWorldRenderer *GetWorldRenderer()
		{
			return mpWorldRenderer.get();
		}

		inline const IWorldRenderer *GetWorldRenderer() const
		{
			return mpWorldRenderer.get();
		}

		int Run(int argc, char **argv);
		inline void Exit()
		{
			mRunning = false;
		}

		friend class GameObject;

	protected:
		const ApplicationSettings mSettings;
		glm::vec4 mClearColor;
		glm::vec4 mAmbientLight;
		uint32_t mFrameRate;
		bool mRunning{false};

		virtual bool ParseCommandLineArguments(int argc, char **argv);
		virtual void OnRegisterComponent() {}
		virtual void OnResize() {}
		virtual void OnStart() {}
		virtual void OnEnd() {}
		virtual void OnMouseButton(MouseButton button, MouseButtonState state) {}
		virtual void OnMouseMove(uint32_t x, uint32_t y) {}
		virtual void OnKeyPress(Key key) {}
		virtual void OnKeyRelease(Key key) {}
		virtual void OnPrintUsage() {}
		virtual void RunMainLoop() = 0;
		void RunMainLoopIteration();
		void WindowResizeCallback(uint32_t width, uint32_t height);
		void MouseButtonCallback(MouseButton button, MouseButtonState state);
		void MouseMoveCallback(uint32_t x, uint32_t y);
		void KeyboardCallback(Key key, bool pressed);
		virtual void OnInitialize() {}
		virtual void OnFinalize() {}

	private:
		static BaseApplication *smpInstance;

		std::string mWindowTitle;
		uint32_t mScreenWidth;
		uint32_t mScreenHeight;
		double mSecondsPerFrame;
		std::unique_ptr<RenderBatchStrategy> mpRenderBatchStrategy;
		std::unique_ptr<IWorldRenderer> mpWorldRenderer{nullptr};
		std::unique_ptr<ImGuiRenderer> mpImGuiRenderer{nullptr};
		Timer mStartTimer;
		Timer mFrameRateTimer;
		size_t mFrameCount{0};
		double mTotalElapsedTime{0};
		double mLastFrameTime{0};
		RenderingStatistics mRenderingStatistics;
		Camera *mpMainCamera{nullptr};
		std::vector<GameObject *> mGameObjects;
		std::vector<Renderable *> mRenderables;
		std::vector<DirectionalLight *> mDirectionalLights;
		std::vector<PointLight *> mPointLights;
		std::vector<Component *> mComponents;
		std::vector<Camera *> mCameras;
		std::vector<Behaviour *> mBehaviours;

		void RegisterGameObject(GameObject *pGameObject);
		void UnregisterGameObject(GameObject *pGameObject);
		void RegisterComponent(Component *pComponent);
		void RegisterCamera(Camera *pCamera);
		void UnregisterComponent(Component *pComponent);
		void Initialize();
		void Finalize();
	};

}

#endif