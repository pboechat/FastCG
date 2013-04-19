#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <Camera.h>
#include <Light.h>
#include <Geometry.h>

#include <string>
#include <vector>
#include <sstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
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

	inline const Camera& GetMainCamera() const
	{
		return mMainCamera;
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
	Camera mMainCamera;
	glm::vec4 mClearColor;
	glm::vec4 mGlobalAmbientLight;

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

	inline void AddGeometry(GeometryPtr geometryPtr)
	{
		mGeometries.push_back(geometryPtr);
	}

	inline void AddLight(LightPtr lightPtr)
	{
		mLights.push_back(lightPtr);
	}

	// TODO:
	//void RemoveGeometry(GeometryPtr geometryPtr);

	// TODO:
	//void RemoveLight(LightPtr lightPtr);

private:
	static Application* s_mpInstance;
	static bool s_mStarted;

	unsigned int mScreenWidth;
	unsigned int mScreenHeight;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	std::string mWindowTitle;
	unsigned int mGLUTWindowHandle;
	std::vector<LightPtr> mLights;
	std::vector<GeometryPtr> mGeometries;

	void SetUpGLUT(int argc, char** argv);
	void SetUpOpenGL();

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
