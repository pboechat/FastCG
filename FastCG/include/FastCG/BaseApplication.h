#ifndef FASTCG_BASE_APPLICATION_H
#define FASTCG_BASE_APPLICATION_H

#include <FastCG/Timer.h>
#include <FastCG/ShaderImporter.h>
#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderingPath.h>
#include <FastCG/MouseButton.h>
#include <FastCG/MaterialDefinitionImporter.h>
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

		virtual bool ParseCommandLineArguments(int argc, char **argv);
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
		Timer mStartTimer;
		Timer mFrameRateTimer;
		size_t mFrameCount{0};
		double mTotalFrameElapsedTime{0};
		double mLastFrameStart{0};
		double mLastCpuElapsedTime{0};
		double mLastGpuElapsedTime{0};
		RenderingStatistics mRenderingStatistics;

		void Initialize();
		void Finalize();
	};

}

#endif