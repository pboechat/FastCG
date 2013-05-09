#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <Camera.h>
#include <Light.h>
#include <Updateable.h>
#include <Drawable.h>
#include <Font.h>
#include <Timer.h>

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

#include <glm/glm.hpp>

class Input;

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
	void Update();
	void Resize(int width, int height);
	void MouseButton(int button, int state, int x, int y);
	void MouseWheel(int button, int direction, int x, int y);
	void MouseMove(int x, int y);
	void Keyboard(int key, int x, int y, bool state);

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
	virtual void BeforeUpdate();
	virtual void AfterUpdate();
	virtual void OnResize();
	virtual void OnStart();
	virtual void OnFinish();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnMouseWheel(int button, int direction, int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnKeyPress(int keyCode);
	virtual void OnKeyRelease(int keyCode);
	virtual void PrintUsage();

#ifdef USE_PROGRAMMABLE_PIPELINE
	void DrawText(const std::string& rText, unsigned int size, int x, int y, FontPtr fontPtr, const glm::vec4& rColor);
#else
	void DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor);
#endif

	inline void AddUpdateable(UpdateablePtr updateablePtr)
	{
		mUpdateables.push_back(updateablePtr);
	}

	void RemoveUpdateable(UpdateablePtr drawablePtr);

	inline void AddDrawable(DrawablePtr drawablePtr)
	{
		mDrawables.push_back(drawablePtr);
	}

	void RemoveDrawable(DrawablePtr drawablePtr);

	inline void AddLight(LightPtr lightPtr)
	{
		mLights.push_back(lightPtr);
	}

private:

	struct DrawTextRequest
	{
		std::string text;
		unsigned int size;
		int x;
		int y;
#ifdef USE_PROGRAMMABLE_PIPELINE
		FontPtr fontPtr;
#endif
		glm::vec4 color;

#ifdef USE_PROGRAMMABLE_PIPELINE
		DrawTextRequest(const std::string& rText, unsigned int size, int x, int y, FontPtr fontPtr, const glm::vec4& rColor)
#else
		DrawTextRequest(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
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
	Input* mpInput;
	unsigned int mScreenWidth;
	unsigned int mScreenHeight;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	std::string mWindowTitle;
	unsigned int mGLUTWindowHandle;
	std::vector<LightPtr> mLights;
	std::vector<UpdateablePtr> mUpdateables;
	std::vector<DrawablePtr> mDrawables;
	Timer mStartTimer;
	Timer mFrameTimer;
	Timer mUpdateTimer;
	unsigned int mElapsedFrames;
	double mElapsedTime;
	std::vector<DrawTextRequest> mDrawTextRequests;

	void SetUpGLUT(int argc, char** argv);
	void SetUpOpenGL();
	void DrawAllTexts();
	
};

void GLUTIdleCallback();
void GLUTDisplayCallback();
void GLUTReshapeWindowCallback(int width, int height);
void GLUTMouseButtonCallback(int button, int state, int x, int y);
void GLUTMouseWheelCallback(int button, int direction, int x, int y);
void GLUTMouseMoveCallback(int x, int y);
void GLUTKeyboardCallback(unsigned char key, int x, int y);
void GLUTKeyboardUpCallback(unsigned char key, int x, int y);
void GLUTSpecialKeysCallback(int key, int x, int y);
void GLUTSpecialKeysUpCallback(int key, int x, int y);
void ExitCallback();
 
#endif
