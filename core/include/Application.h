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
#include <RenderingStrategy.h>
#include <RenderBatchingStrategy.h>
#include <RenderingStatistics.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>

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

class Input;

class Application
{
public:
	Application(const std::string& rWindowTitle, int screenWidth, int screenHeight);
	virtual ~Application();

	inline static Application* GetInstance()
	{
		return s_mpInstance;
	}

	void Run(int argc, char** argv);
	void Exit();

	inline unsigned int GetScreenWidth() const
	{
		return mScreenWidth;
	}

	inline unsigned int GetScreenHeight() const
	{
		return mScreenHeight;
	}

	inline Camera* GetMainCamera()
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

#ifdef USE_PROGRAMMABLE_PIPELINE
	void DrawText(const std::string& rText, unsigned int size, int x, int y, Font* pFont, const glm::vec4& rColor);
#endif
	void DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor);
	void BeforeMeshFilterChange(MeshFilter* pMeshFilter);
	void AfterMeshFilterChange(MeshFilter* pMeshFilter);

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
	bool mShowRenderingStatistics;
#ifdef USE_PROGRAMMABLE_PIPELINE
	Font* mpStandardFont;
#endif

	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void SetUpViewport();
	virtual void OnResize();
	virtual void OnStart();
	virtual void OnEnd();
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
		Font* pFont;
#endif
		glm::vec4 color;

#ifdef USE_PROGRAMMABLE_PIPELINE
		DrawTextRequest(const std::string& rText, unsigned int size, int x, int y, Font* pFont, const glm::vec4& rColor)
#else
		DrawTextRequest(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
#endif
		{
			text = rText;
			this->size = size;
			this->x = x;
			this->y = y;
#ifdef USE_PROGRAMMABLE_PIPELINE
			this->pFont = pFont;
#endif
			color = rColor;
		}
	};

	static Application* s_mpInstance;

	unsigned int mScreenWidth;
	unsigned int mScreenHeight;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	float mAspectRatio;
	std::string mWindowTitle;
	unsigned int mGLUTWindowHandle;
	GameObject* mpInternalGameObject;
	std::vector<GameObject*> mGameObjects;
	std::vector<Camera*> mCameras;
	std::vector<Light*> mLights;
	std::vector<MeshFilter*> mMeshFilters;
	std::vector<Behaviour*> mBehaviours;
	std::vector<LineRenderer*> mLineRenderers;
	std::vector<PointsRenderer*> mPointsRenderers;
	std::vector<Component*> mComponents;
	std::vector<RenderBatch*> mRenderBatches;
	Timer mStartTimer;
	Timer mFrameTimer;
	Timer mUpdateTimer;
	unsigned int mElapsedFrames;
	double mElapsedTime;
	std::vector<DrawTextRequest*> mDrawTextRequests;
	Input* mpInput;
	RenderingStrategy* mpRenderingStrategy;
	RenderBatchingStrategy* mpRenderBatchingStrategy;
	RenderingStatistics mRenderingStatistics;

	void SetUpGLUT(int argc, char** argv);
	void SetUpOpenGL();
	void SetMainCamera(Camera* pCamera);
	void RegisterGameObject(GameObject* pGameObject);
	void UnregisterGameObject(GameObject* pGameObject);
	void RegisterComponent(Component* pComponent);
	void RegisterCamera(Camera* pCamera);
	void UnregisterComponent(Component* pComponent);
	void DrawAllTexts();
	void ShowFPS();
	void ShowRenderingStatistics();
	void Update();
	void Render();
	void Resize(int width, int height);
	void MouseButton(int button, int state, int x, int y);
	void MouseWheel(int button, int direction, int x, int y);
	void MouseMove(int x, int y);
	void Keyboard(int key, int x, int y, bool state);

};

#endif
