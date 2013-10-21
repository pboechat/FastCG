#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <GameObject.h>
#include <Component.h>
#include <Camera.h>
#include <Light.h>
#include <DirectionalLight.h>
#include <PointLight.h>
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

#include <windows.h>
#include <windowsx.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class Input;
class ModelImporter;

////////////////////////////////////////////////////////////////////////////////////////////////////
class Application
{
public:
	static const std::string DEFERRED_RENDERING_SHADERS_FOLDER;
	static const std::string SHADERS_FOLDER;
	static const std::string FONTS_FOLDER;
	static const std::string DEFAULT_FONT_NAME;

	Application(const std::string& rWindowTitle, unsigned int screenWidth, unsigned int screenHeight, unsigned int frameRate, bool deferredRendering = false, const std::string& rGlobalResourcePath = "");
	virtual ~Application();

	inline static Application* GetInstance()
	{
		return s_mpInstance;
	}

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

	inline RenderingStrategy* GetRenderingStrategy()
	{
		return mpRenderingStrategy;
	}

	int Run(int argc, char** argv);
	void Exit();
	void DrawText(const std::string& rText, unsigned int size, int x, int y, Font* pFont, const glm::vec4& rColor);
	void DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor);
	void BeforeMeshFilterChange(MeshFilter* pMeshFilter);
	void AfterMeshFilterChange(MeshFilter* pMeshFilter);

	friend class GameObject;
	friend LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	static const std::string WINDOW_CLASS_NAME;
	static const unsigned int BYTES_PER_PIXEL;
	static const unsigned int COLOR_BUFFER_BITS;
	static const unsigned int DEPTH_BUFFER_BITS;
	static const unsigned int HAS_ALPHA;
	static const PIXELFORMATDESCRIPTOR PIXEL_FORMAT_DESCRIPTOR;

	Camera* mpMainCamera;
	glm::vec4 mClearColor;
	glm::vec4 mGlobalAmbientLight;
	bool mShowFPS;
	bool mShowRenderingStatistics;
	Font* mpStandardFont;

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
		Font* pFont;
		glm::vec4 color;

		DrawTextRequest(const std::string& rText, unsigned int size, int x, int y, Font* pFont, const glm::vec4& rColor)
		{
			text = rText;
			this->size = size;
			this->x = x;
			this->y = y;
			this->pFont = pFont;
			color = rColor;
		}
	};

	static Application* s_mpInstance;

	unsigned int mScreenWidth;
	unsigned int mScreenHeight;
	unsigned int mFrameRate;
	double mSecondsPerFrame;
	bool mDeferredRendering;
	std::string mGlobalResourcePath;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	float mAspectRatio;
	std::string mWindowTitle;
	HINSTANCE mApplicationHandle;
	HWND mWindowHandle;
	HDC mDeviceContextHandle;
	int mPixelFormat;
	HGLRC mOpenGLRenderingContextHandle;
	GameObject* mpInternalGameObject;
	std::vector<GameObject*> mGameObjects;
	std::vector<Camera*> mCameras;
	std::vector<Light*> mLights;
	std::vector<DirectionalLight*> mDirectionalLights;
	std::vector<PointLight*> mPointLights;
	std::vector<MeshFilter*> mMeshFilters;
	std::vector<Behaviour*> mBehaviours;
	std::vector<LineRenderer*> mLineRenderers;
	std::vector<PointsRenderer*> mPointsRenderers;
	std::vector<Component*> mComponents;
	std::vector<RenderBatch*> mRenderBatches;
	Timer mApplicationTimer;
	Timer mFrameRateTimer;
	unsigned int mElapsedFrames;
	double mTotalElapsedTime;
	std::vector<DrawTextRequest*> mDrawTextRequests;
	Input* mpInput;
	ModelImporter* mpModelImporter;
	RenderingStrategy* mpRenderingStrategy;
	RenderBatchingStrategy* mpRenderBatchingStrategy;
	RenderingStatistics mRenderingStatistics;

	void Dispose();
	WNDCLASSEX CreateWindowClass();
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
	void Keyboard(int key, bool state);
	
};

#endif
