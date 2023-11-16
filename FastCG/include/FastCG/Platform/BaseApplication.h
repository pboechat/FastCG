#ifndef FASTCG_BASE_APPLICATION_H
#define FASTCG_BASE_APPLICATION_H

#include <FastCG/Rendering/RenderingStatistics.h>
#include <FastCG/Rendering/MaterialDefinitionImporter.h>
#include <FastCG/Input/MouseButton.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Graphics/ShaderImporter.h>
#include <FastCG/Graphics/RenderingPath.h>
#include <FastCG/Core/Version.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Colors.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace FastCG
{
	using ImportCallback = void (*)(void);

	constexpr uint32_t UNLOCKED_FRAMERATE = ~0u;
	constexpr ImportCallback DEFAULT_IMPORT_CALLBACKS[] = {&ShaderImporter::Import,
														   &MaterialDefinitionImporter::Import};

	struct ApplicationSettings
	{
		std::string windowTitle{};
		uint32_t screenWidth{1024};
		uint32_t screenHeight{768};
		uint32_t frameRate{60};
		uint32_t maxSimultaneousFrames{3};
		bool vsync{false};
		struct
		{
			RenderingPath path{RenderingPath::FORWARD};
			glm::vec4 clearColor{Colors::BLACK};
			glm::vec4 ambientLight{Colors::BLACK};
		} rendering{};
		struct
		{
			std::vector<std::string> bundles{};
			std::vector<ImportCallback> importCallbacks{DEFAULT_IMPORT_CALLBACKS, DEFAULT_IMPORT_CALLBACKS + FASTCG_ARRAYSIZE(DEFAULT_IMPORT_CALLBACKS)};
		} assets{};
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

		inline uint32_t GetFrameRate() const
		{
			return mFrameRate;
		}

		inline void SetFrameRate(uint32_t frameRate)
		{
			mFrameRate = frameRate;
			mSecondsPerFrame = 1 / (double)frameRate;
		}

		inline RenderingPath GetRenderingPath() const
		{
			return mSettings.rendering.path;
		}

		int Run();
		int Run(int argc, char **argv);

		inline void Exit()
		{
			mRunning = false;
		}

		virtual uint64_t GetNativeKey(Key key) const = 0;

	protected:
		const ApplicationSettings mSettings;
		uint32_t mFrameRate;
		bool mRunning{false};

		virtual bool ParseCommandLineArguments(int argc, char **argv)
		{
			FASTCG_UNUSED(argc);
			FASTCG_UNUSED(argv);
			return true;
		}
		virtual void OnResize() {}
		virtual void OnStart() {}
		virtual void OnEnd() {}
		virtual void OnMouseButton(MouseButton button, MouseButtonState state)
		{
			FASTCG_UNUSED(button);
			FASTCG_UNUSED(state);
		}
		virtual void OnMouseMove(uint32_t x, uint32_t y)
		{
			FASTCG_UNUSED(x);
			FASTCG_UNUSED(y);
		}
		virtual void OnKeyPress(Key key)
		{
			FASTCG_UNUSED(key);
		}
		virtual void OnKeyRelease(Key key)
		{
			FASTCG_UNUSED(key);
		}
		virtual void OnPrintUsage() {}
		virtual void RunMainLoop() = 0;
		void RunMainLoopIteration(double osTime);
		void WindowResizeCallback(uint32_t width, uint32_t height);
		void MouseButtonCallback(MouseButton button, MouseButtonState state);
		void MouseMoveCallback(uint32_t x, uint32_t y);
		void KeyboardCallback(Key key, bool pressed);
		virtual void OnPreInitialize() {}
		virtual void OnPostInitialize() {}
		virtual void OnPreFinalize() {}
		virtual void OnPostFinalize() {}

	private:
		static BaseApplication *smpInstance;

		std::string mWindowTitle;
		uint32_t mScreenWidth;
		uint32_t mScreenHeight;
		double mSecondsPerFrame;
		size_t mFrameCount{0};
		double mLastAppStart{0};
		double mLastAppElapsedTime{0};
		double mLastGpuElapsedTime{0};
		double mLastPresentElapsedTime{0};
		double mLastWaitTime{0};
		RenderingStatistics mRenderingStatistics;
		void Initialize();
		void Finalize();
	};

}

#define FASTCG_LOG_BUILD_PROPERTIES()                                       \
	FASTCG_LOG_DEBUG(Build, "Platform: %s", FASTCG_PLATFORM);               \
	FASTCG_LOG_DEBUG(Build, "Graphics system: %s", FASTCG_GRAPHICS_SYSTEM); \
	FASTCG_LOG_DEBUG(Build, "FastCG version: %d.%d.%d", FastCG::MAJOR_VERSION, FastCG::MINOR_VERSION, FastCG::PATCH_VERSION)

#if defined FASTCG_ANDROID
#define FASTCG_MAIN(appType)                   \
	void android_main(android_app *androidApp) \
	{                                          \
		FASTCG_LOG_BUILD_PROPERTIES();         \
		appType app;                           \
		app.SetAndroidApp(androidApp);         \
		app.Run();                             \
	}
#else
#define FASTCG_MAIN(appType)           \
	int main(int argc, char **argv)    \
	{                                  \
		FASTCG_LOG_BUILD_PROPERTIES(); \
		appType app;                   \
		return app.Run(argc, argv);    \
	}
#endif

#endif