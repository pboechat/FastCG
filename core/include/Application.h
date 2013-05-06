#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <Camera.h>
#include <Light.h>
#include <TriangleMesh.h>
#include <Font.h>
#include <Timer.h>

#include <string>
#include <vector>
#include <sstream>

#include <glm/glm.hpp>

class Application
{
public:
	Application(const std::string& rWindowTitle, int screenWidth, int screenHeight);
	virtual ~Application();

	static Application* GetInstance()
	{
		return s_mpInstance;
	}

	static bool HasStarted();

	void Run(int argc, char** argv);
	void Quit();
	void Display();
	void Resize(int width, int height);
	void MouseButton(int button, int state, int x, int y);
	void MouseWheel(int button, int direction, int x, int y);
	void MouseMove(int x, int y);
	void Keyboard(int key, int x, int y);

	inline unsigned int GetScreenWidth() const
	{
		return mScreenWidth;
	}

	inline unsigned int GetScreenHeight() const
	{
		return mScreenHeight;
	}

	inline float GetAspectRatio() const
	{
		return mScreenWidth / (float) mScreenHeight;
	}

	inline CameraPtr GetMainCamera() const
	{
		return mMainCameraPtr;
	}

	inline const glm::vec4& GetGlobalAmbientLight() const
	{
		return mGlobalAmbientLight;
	}

	inline const std::vector<LightPtr>& GetLights() const
	{
		return mLights;
	}

protected:
	CameraPtr mMainCameraPtr;
	glm::vec4 mClearColor;
	glm::vec4 mGlobalAmbientLight;
	bool mShowFPS;
#ifdef USE_PROGRAMMABLE_PIPELINE
	FontPtr mStandardFontPtr;
#endif

	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void SetUpViewport();
	virtual void BeforeDisplay();
	virtual void AfterDisplay();
	virtual void OnResize();
	virtual void OnStart();
	virtual void OnFinish();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnMouseWheel(int button, int direction, int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnKeyPress(int key);
	virtual void PrintUsage();

#ifdef USE_PROGRAMMABLE_PIPELINE
	void DrawText(const std::string& rText, unsigned int size, unsigned int x, unsigned int y, FontPtr fontPtr, const glm::vec4& rColor);
#else
	void DrawText(const std::string& rText, unsigned int size, unsigned int x, unsigned int y, const glm::vec4& rColor);
#endif

	inline void AddDrawable(DrawablePtr drawablePtr)
	{
		mDrawables.push_back(drawablePtr);
	}

	inline void AddLight(LightPtr lightPtr)
	{
		mLights.push_back(lightPtr);
	}

private:

	struct DrawTextRequest
	{
		std::string text;
		unsigned int size;
		unsigned int x;
		unsigned int y;
#ifdef USE_PROGRAMMABLE_PIPELINE
		FontPtr fontPtr;
#endif
		glm::vec4 color;

#ifdef USE_PROGRAMMABLE_PIPELINE
		DrawTextRequest(const std::string& rText, unsigned int size, unsigned int x, unsigned int y, FontPtr fontPtr, const glm::vec4& rColor)
#else
		DrawTextRequest(const std::string& rText, unsigned int size, unsigned int x, unsigned int y, const glm::vec4& rColor)
#endif
		{
			text = rText;
			this->size = size;
			this->x = x;
			this->y = y;
#ifdef USE_PROGRAMMABLE_PIPELINE
			this->fontPtr = fontPtr;
#endif
			color = rColor;
		}
	};

	static Application* s_mpInstance;
	static bool s_mStarted;
	unsigned int mScreenWidth;
	unsigned int mScreenHeight;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	std::string mWindowTitle;
	unsigned int mGLUTWindowHandle;
	std::vector<LightPtr> mLights;
	std::vector<DrawablePtr> mDrawables;
	Timer mFrameTimer;
	unsigned int mElapsedFrames;
	double mElapsedTime;
	std::vector<DrawTextRequest> mDrawTextRequests;

	void SetUpGLUT(int argc, char** argv);
	void SetUpOpenGL();
	void DrawAllTexts();

};

void GLUTDisplayCallback();
void GLUTReshapeWindowCallback(int width, int height);
void GLUTMouseButtonCallback(int button, int state, int x, int y);
void GLUTMouseWheelCallback(int button, int direction, int x, int y);
void GLUTMouseMoveCallback(int x, int y);
void GLUTKeyboardCallback(unsigned char key, int x, int y);
void GLUTSpecialKeysCallback(int key, int x, int y);
void ExitCallback();
 
#endif
