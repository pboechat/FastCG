#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <string>
#include <vector>
#include <sstream>

#include "Camera.h"

class Application
{
public:
	Application(const std::string& rWindowTitle, int screenWidth, int screenHeight);
	virtual ~Application();

	static Application* GetInstance();
	static bool HasStarted();

	void Run(int argc, char** argv);
	void Quit();
	void Display();
	void Resize(int width, int height);
	void MouseButton(int button, int state, int x, int y);
	void MouseMove(int x, int y);
	void Keyboard(int key, int x, int y);

	inline float GetAspectRatio() const
	{
		return mScreenWidth / (float) mScreenHeight;
	}

	inline const Camera& GetMainCamera() const
	{
		return mainCamera;
	}

protected:
	int mScreenWidth;
	int mScreenHeight;
	float mHalfScreenWidth;
	float mHalfScreenHeight;
	Camera mainCamera;

	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void SetUpViewport();
	virtual void OnDisplay();
	virtual void OnResize();
	virtual bool OnStart();
	virtual void OnFinish();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnKeyPress(int key);
	virtual void PrintUsage();

	/*int GetMaximumTextureSize() const;
	unsigned int AllocateTexture(unsigned int target, unsigned int width, unsigned int height, int internalFormat, int textureFormat);
	unsigned int AllocateTexture(unsigned int target, unsigned int width, unsigned int height, int internalFormat, int textureFormat, float* pData);
	unsigned int AllocateFBO();
	unsigned int AllocatePBO(unsigned int size, unsigned int usage, float* pData = 0);
	unsigned int AllocateVBO(unsigned int size, unsigned int usage, float* pData = 0);
	unsigned int AllocateBO();
	void DeallocateFBO(unsigned int fboId);
	void DeallocateBO(unsigned int vboId);
	void DeallocateTexture(unsigned int textureId);
	void TransferToTexture(unsigned int textureId, unsigned int textureTarget, unsigned int textureWidth, unsigned int textureHeight, unsigned int textureFormat, float* pBuffer) const;
	void TransferFromTexture(unsigned int textureId, unsigned int target, unsigned int format, float* pBuffer) const;
	void TransferFromBuffer(unsigned int bufferId, unsigned int width, unsigned int height, unsigned int format, float* pBuffer) const;*/

private:
	static Application* s_mpInstance;
	static bool s_mStarted;

	std::string mWindowTitle;
	unsigned int mGLUTWindowHandle;

	void SetUpGLUT(int argc, char** argv);

};

void GLUTDisplayCallback();
void GLUTReshapeWindowCallback(int width, int height);
void GLUTMouseButtonCallback(int button, int state, int x, int y);
void GLUTMouseMoveCallback(int x, int y);
void GLUTKeyboardCallback(unsigned char key, int x, int y);
void GLUTSpecialKeysCallback(int key, int x, int y);
void ExitCallback();

// ====================================================================================================

#define EXPAND(x) x
#define CHECK_FOR_OPENGL_ERRORS() \
	{ \
		unsigned int __errorCode; \
		if ((__errorCode = glGetError()) != GL_NO_ERROR) \
		{ \
			const unsigned char* __pErrorMessage = gluErrorString(__errorCode); \
			std::stringstream __stringStream; \
			if (__pErrorMessage) \
			{ \
				__stringStream << __pErrorMessage; \
			} \
			__stringStream << "\n" << "(error code: %d)"; \
			EXPAND(THROW_EXCEPTION(OpenGLException, __stringStream.str().c_str(), __errorCode)); \
		} \
	} \

#endif
