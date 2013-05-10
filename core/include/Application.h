#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <GameObject.h>
#include <Component.h>
#include <Camera.h>
#include <Light.h>
#include <Renderer.h>
#include <LineRenderer.h>
#include <PointsRenderer.h>
#include <Behaviour.h>
#include <MeshFilter.h>
#include <Font.h>
#include <Shader.h>
#include <Timer.h>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>

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

#ifdef USE_PROGRAMMABLE_PIPELINE
	void DrawText(const std::string& rText, unsigned int size, int x, int y, FontPtr fontPtr, const glm::vec4& rColor);
#endif

	void DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor);

	inline unsigned int GetScreenWidth() const
	{
		return mScreenWidth;
	}

	inline unsigned int GetScreenHeight() const
	{
		return mScreenHeight;
	}

	inline CameraPtr GetMainCamera() const
	{
		return mpMainCamera;
	}

	inline const glm::vec4& GetClearColor() const
	{
		return mClearColor;
	}

	inline void SetClearColor(const glm::vec4& clearColor)
	{
		mClearColor = clearColor;
	}

	friend void GLUTIdleCallback();
	friend void GLUTDisplayCallback();
	friend void GLUTReshapeWindowCallback(int, int);
	friend void GLUTMouseButtonCallback(int, int, int, int);
	friend void GLUTMouseWheelCallback(int, int, int, int);
	friend void GLUTMouseMoveCallback(int, int);
	friend void GLUTKeyboardCallback(unsigned char, int, int);
	friend void GLUTKeyboardUpCallback(unsigned char, int, int);
	friend void GLUTSpecialKeysCallback(int, int, int);
	friend void GLUTSpecialKeysUpCallback(int, int, int);
	friend void ExitCallback();

	friend class GameObject;

protected:
	Camera* mpMainCamera;
	glm::vec4 mClearColor;
	glm::vec4 mGlobalAmbientLight;
	bool mShowFPS;
#ifdef USE_PROGRAMMABLE_PIPELINE
	FontPtr mStandardFontPtr;
#endif

	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void SetUpViewport();
	virtual void OnResize();
	virtual void OnStart();
	virtual void OnFinish();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnMouseWheel(int button, int direction, int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnKeyPress(int keyCode);
	virtual void OnKeyRelease(int keyCode);
	virtual void PrintUsage();

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

	struct RenderingGroup
	{
		Material* pMaterial;
		std::vector<MeshFilter*> meshFilters;
	};

	static Application* s_mpInstance;
	static bool s_mStarted;

	unsigned int mScreenWidth;
	unsigned int mScreenHeight;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	float mAspectRatio;
	std::string mWindowTitle;
	unsigned int mGLUTWindowHandle;
	std::vector<GameObjectPtr> mGameObjects;
	std::vector<Camera*> mCameras;
	std::vector<Light*> mLights;
	std::vector<MeshFilter*> mMeshFilters;
	std::vector<Behaviour*> mBehaviours;
	std::vector<Component*> mComponents;
	std::vector<LineRenderer*> mLineRenderers;
	std::vector<PointsRenderer*> mPointsRenderers;
	std::vector<RenderingGroup> mRenderingGroups;
#ifdef USE_PROGRAMMABLE_PIPELINE
	ShaderPtr mLineStripShaderPtr;
	ShaderPtr mPointsShaderPtr;
#endif
	Timer mStartTimer;
	Timer mFrameTimer;
	Timer mUpdateTimer;
	unsigned int mElapsedFrames;
	double mElapsedTime;
	std::vector<DrawTextRequest> mDrawTextRequests;
	Input* mpInput;
	GameObject* mpInternalGameObject;

	void SetUpGLUT(int argc, char** argv);
	void SetUpOpenGL();
	void DrawAllTexts();
	void Update();
	void Resize(int width, int height);
	void MouseButton(int button, int state, int x, int y);
	void MouseWheel(int button, int direction, int x, int y);
	void MouseMove(int x, int y);
	void Keyboard(int key, int x, int y, bool state);
	void SetMainCamera(Camera* pCamera);
	void RegisterGameObject(const GameObjectPtr& rGameObjectPtr);
	void UnregisterGameObject(const GameObjectPtr& rGameObjectPtr);
	void RegisterComponent(const ComponentPtr& rComponentPtr);
	void RegisterCamera(Camera* pCamera);
	void RegisterMeshFilter(MeshFilter* pMeshFilter);
	void UnregisterComponent(const ComponentPtr& rComponentPtr);
	void RemoveFromMeshRenderingGroups(MeshFilter* pMeshFilter);
#ifdef USE_PROGRAMMABLE_PIPELINE
	void SetUpShaderLights(Shader* pShader);
#endif

};
 
#endif
