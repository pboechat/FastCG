#ifndef FASTCG_APPLICATION_H_
#define FASTCG_APPLICATION_H_

#include <FastCG/Timer.h>
#include <FastCG/Shader.h>
#include <FastCG/RenderingPathStrategy.h>
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
#include <FastCG/DirectionalLight.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>

#include <glm/glm.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <vector>
#include <cstdint>

#ifdef _WIN32
LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
#endif

namespace FastCG
{
	class Input;
	class ModelImporter;

	enum class RenderingPath : uint8_t
	{
		RP_FORWARD_RENDERING = 0,
		RP_DEFERRED_RENDERING,

	};

	struct ApplicationSettings
	{
		std::string windowTitle;
		uint32_t screenWidth{ 1024 };
		uint32_t screenHeight{ 768 };
		uint32_t frameRate{ 60 };
		RenderingPath renderingPath{ RenderingPath::RP_FORWARD_RENDERING };
		std::string assetsPath{ "../assets" };
		glm::vec4 clearColor{ Colors::BLACK };
		glm::vec4 ambientLight{ Colors::BLACK };
		bool showFPS{ false };
		bool showRenderingStatistics{ false };

	};

	class Application
	{
	public:
		Application(const ApplicationSettings& settings);
		virtual ~Application();

		inline static Application* GetInstance()
		{
			return s_mpInstance;
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

		inline Camera* GetMainCamera()
		{
			return mpMainCamera;
		}

		void SetMainCamera(Camera* pCamera);

		inline const glm::vec4& GetClearColor() const
		{
			return mClearColor;
		}

		inline void SetClearColor(const glm::vec4& clearColor)
		{
			mClearColor = clearColor;
		}

		inline std::shared_ptr<RenderingPathStrategy> GetRenderingStrategy() const
		{
			return mpRenderingPathStrategy;
		}

		int Run(int argc, char** argv);
		void Exit();
		void DrawText(const std::string& rText, uint32_t x, uint32_t y, const std::shared_ptr<Font>& pFont, const glm::vec4& rColor);
		void DrawText(const std::string& rText, uint32_t x, uint32_t y, const glm::vec4& rColor);
		void BeforeMeshFilterChange(MeshFilter* pMeshFilter);
		void AfterMeshFilterChange(MeshFilter* pMeshFilter);

		friend class GameObject;
#ifdef _WIN32
		friend LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
#endif

	protected:
		glm::vec4 mClearColor;
		glm::vec4 mAmbientLight;
		bool mShowFPS;
		bool mShowRenderingStatistics;

		virtual bool ParseCommandLineArguments(int argc, char** argv);
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

	private:
		struct DrawTextRequest
		{
			std::string text;
			uint32_t x;
			uint32_t y;
			std::shared_ptr<Font> pFont;
			glm::vec4 color;

		};

		static Application* s_mpInstance;

		std::string mWindowTitle;
		uint32_t mScreenWidth;
		uint32_t mScreenHeight;
		RenderingPath mRenderingPath;
		std::string mAssetsPath;
		double mSecondsPerFrame;
		bool mRunning{ false };
		std::shared_ptr<Font> mpStandardFont{ nullptr };
		std::shared_ptr<RenderingPathStrategy> mpRenderingPathStrategy{ nullptr };
		std::unique_ptr<RenderBatchingStrategy> mpRenderBatchingStrategy{ nullptr };
		std::unique_ptr<Input> mpInput{ nullptr };
		Timer mStartTimer;
		Timer mFrameRateTimer;
		uint32_t mElapsedFrames{ 0 };
		double mTotalElapsedTime{ 0 };
		double mLastFrameTime{ 0 };
		RenderingStatistics mRenderingStatistics;
		GameObject* mpInternalGameObject{ nullptr };
		Camera* mpMainCamera{ nullptr };
		std::vector<GameObject*> mGameObjects;
		std::vector<Camera*> mCameras;
		std::vector<DirectionalLight*> mDirectionalLights;
		std::vector<PointLight*> mPointLights;
		std::vector<MeshFilter*> mMeshFilters;
		std::vector<Behaviour*> mBehaviours;
		std::vector<LineRenderer*> mLineRenderers;
		std::vector<PointsRenderer*> mPointsRenderers;
		std::vector<Component*> mComponents;
		std::vector<std::unique_ptr<RenderBatch>> mRenderBatches;
		std::vector<DrawTextRequest> mDrawTextRequests;
#ifdef _WIN32
		HINSTANCE mHInstance{ 0 };
		HWND mHWnd{ 0 };
		HDC mHDC{ 0 };
		HGLRC mHGLRC{ 0 };
#endif

		void SetUpPresentation();
		void TearDownPresentation();
		void SetUpOpenGL();
		void CreateOpenGLContext();
		void TearDownOpenGL();
		void DestroyOpenGLContext();
		void RunMainLoop();
		void RegisterGameObject(GameObject* pGameObject);
		void UnregisterGameObject(GameObject* pGameObject);
		void RegisterComponent(Component* pComponent);
		void RegisterCamera(Camera* pCamera);
		void UnregisterComponent(Component* pComponent);
		void DrawAllTexts();
		void ShowFPS();
		void ShowRenderingStatistics();
		void Update();
		void Present();
		void Render();
		void WindowResizeCallback(int width, int height);
		void MouseButtonCallback(MouseButton button, MouseButtonState state, int x, int y);
		void MouseWheelCallback(int direction, int x, int y);
		void MouseMoveCallback(int x, int y);
		void KeyboardCallback(int key, bool pressed);

	};

}

#endif
