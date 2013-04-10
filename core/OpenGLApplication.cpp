#include "include/OpenGLApplication.h"
#include "include/OpenGLExceptions.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glext.h>

#include <algorithm>

OpenGLApplication* OpenGLApplication::s_mpInstance = NULL;
bool OpenGLApplication::s_mStarted = false;

OpenGLApplication::OpenGLApplication(const std::string& rWindowTitle, int screenWidth, int screenHeight)
	:
	mWindowTitle(rWindowTitle),
	mScreenWidth(screenWidth),
	mScreenHeight(screenHeight),
	mHalfScreenWidth(screenWidth / 2.0f),
	mHalfScreenHeight(screenHeight / 2.0f)
{
	s_mpInstance = this;
}

OpenGLApplication::~OpenGLApplication()
{
	if (HasStarted())
	{
		Quit();
	}
}

void OpenGLApplication::PrintUsage()
{
}

void OpenGLApplication::Run(int argc, char** argv)
{
	atexit(ExitCallback);

	if (!ParseCommandLineArguments(argc, argv))
	{
		PrintUsage();
		return;
	}
	
	
	SetUpGLUT(argc, argv);
	SetUpViewport();

	if (OnStart())
	{
		glutMainLoop();
		s_mStarted = true;
	}
}

bool OpenGLApplication::ParseCommandLineArguments(int argc, char** argv)
{
	return true;
}

void OpenGLApplication::SetUpGLUT(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(mScreenWidth, mScreenHeight);
	mGLUTWindowHandle = glutCreateWindow(mWindowTitle.c_str());
	glewInit();
	glutDisplayFunc(GLUTDisplayCallback);
	glutIdleFunc(GLUTDisplayCallback);
	glutReshapeFunc(GLUTReshapeWindowCallback);
	glutMouseFunc(GLUTMouseButtonCallback);
	glutMotionFunc(GLUTMouseMoveCallback);
	glutKeyboardFunc(GLUTKeyboardCallback);
	glutSpecialFunc(GLUTSpecialKeysCallback);
}

void OpenGLApplication::Display()
{
	OnDisplay();
}

void OpenGLApplication::Resize(int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	mHalfScreenWidth = width / 2.0f;
	mHalfScreenHeight = height / 2.0f;
	SetUpViewport();
	OnResize();
}

bool OpenGLApplication::OnStart()
{
	return true;
}

void OpenGLApplication::OnDisplay()
{
}

void OpenGLApplication::OnResize()
{
}

void OpenGLApplication::OnFinish()
{
}

OpenGLApplication* OpenGLApplication::GetInstance()
{
	return s_mpInstance;
}

int OpenGLApplication::GetMaximumTextureSize() const
{
	int maximumTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maximumTextureSize);
	return maximumTextureSize;
}

unsigned int OpenGLApplication::AllocateFBO()
{
	unsigned int fboId;

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	mFBOIds.push_back(fboId);

	return fboId;
}

unsigned int OpenGLApplication::AllocatePBO(unsigned int size, unsigned int usage, float* pData)
{
	unsigned int vboId = AllocateBO();

	glBindBuffer(GL_PIXEL_PACK_BUFFER, vboId);
	glBufferData(GL_PIXEL_PACK_BUFFER, size * 4 * sizeof(float), pData, usage);

	return vboId;
}

unsigned int OpenGLApplication::AllocateVBO(unsigned int size, unsigned int usage, float* pData)
{
	unsigned int vboId = AllocateBO();

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, size * 4 * sizeof(float), pData, usage);

	return vboId;
}

unsigned int OpenGLApplication::AllocateBO()
{
	unsigned int boId;

	glGenBuffers(1, &boId);

	mBOIds.push_back(boId);

	return boId;
}

unsigned int OpenGLApplication::AllocateTexture(unsigned int target, unsigned int width, unsigned int height, int internalFormat, int textureFormat)
{
	return AllocateTexture(target, width, height, internalFormat, textureFormat, NULL);
}

unsigned int OpenGLApplication::AllocateTexture(unsigned int target, unsigned int width, unsigned int height, int internalFormat, int textureFormat, float* pData)
{
	unsigned int textureId;

	glGenTextures(1, &textureId);
	glBindTexture(target, textureId);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(target, 0, internalFormat, width, height, 0, textureFormat, GL_FLOAT, pData);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	mTexturesIds.push_back(textureId);

	return textureId;
}

void OpenGLApplication::SetUpViewport()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, mScreenWidth / (float) mScreenHeight, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}

void OpenGLApplication::Quit()
{
	s_mStarted = false;
	OnFinish();
	DeallocateFBOs();
	DeallocateBOs();
	DeallocateTextures();
	glutDestroyWindow(mGLUTWindowHandle);
}

void OpenGLApplication::DeallocateFBOs()
{
	for (unsigned int i = 0; i < mFBOIds.size(); i++)
	{
		DeallocateFBO(mFBOIds[i]);
	}
}

void OpenGLApplication::DeallocateBOs()
{
	for (unsigned int i = 0; i < mBOIds.size(); i++)
	{
		DeallocateBO(mBOIds[i]);
	}
}

void OpenGLApplication::DeallocateTextures()
{
	for (unsigned int i = 0; i < mTexturesIds.size(); i++)
	{
		DeallocateTexture(mTexturesIds[i]);
	}
}

void OpenGLApplication::DeallocateFBO(unsigned int fboId)
{
	glDeleteFramebuffers(1, &fboId);
	std::vector<unsigned int>::iterator iterator = std::find(mFBOIds.begin(), mFBOIds.end(), fboId);
	mFBOIds.erase(iterator);
}

void OpenGLApplication::DeallocateBO(unsigned int vboId)
{
	glDeleteBuffers(1, &vboId);
	std::vector<unsigned int>::iterator iterator = std::find(mBOIds.begin(), mBOIds.end(), vboId);
	mBOIds.erase(iterator);
}

bool OpenGLApplication::HasStarted()
{
	return s_mStarted;
}

void OpenGLApplication::DeallocateTexture(unsigned int textureId)
{
	glDeleteTextures(1, &textureId);
	std::vector<unsigned int>::iterator iterator = std::find(mTexturesIds.begin(), mTexturesIds.end(), textureId);
	mTexturesIds.erase(iterator);
}

void OpenGLApplication::TransferToTexture(unsigned int textureId, unsigned int textureTarget, unsigned int textureWidth, unsigned int textureHeight, unsigned int textureFormat, float* pBuffer) const
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, textureTarget, textureId, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glRasterPos2i(0, 0);
	glDrawPixels(textureWidth, textureHeight, textureFormat, GL_FLOAT, pBuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, textureTarget, 0, 0);

	// TODO: better performance in NVIDIA cards
	//glBindTexture(textureTarget, textureId);
	//glTexSubImage2D(textureTarget, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_FLOAT, pBuffer);
}

void OpenGLApplication::TransferFromBuffer(unsigned int bufferId, unsigned int width, unsigned int height, unsigned int format, float* pBuffer) const
{
	glReadBuffer(bufferId);
	glReadPixels(0, 0, width, height, format, GL_FLOAT, pBuffer);
}

void OpenGLApplication::TransferFromTexture(unsigned int textureId, unsigned int target, unsigned int format, float* pBuffer) const
{
	glBindTexture(target, textureId);
	glGetTexImage(target, 0, format, GL_FLOAT, pBuffer);
}

void OpenGLApplication::MouseButton(int button, int state, int x, int y)
{
	OnMouseButton(button, state, x, y);
}

void OpenGLApplication::MouseMove(int x, int y)
{
	OnMouseMove(x, y);
}

void OpenGLApplication::Keyboard(int key, int x, int y)
{
	OnKeyPress(key);
}

void OpenGLApplication::OnMouseButton(int button, int state, int x, int y)
{
}

void OpenGLApplication::OnMouseMove(int x, int y)
{
}

void OpenGLApplication::OnKeyPress(int key)
{
}

void GLUTDisplayCallback()
{
	OpenGLApplication::GetInstance()->Display();
}

void GLUTReshapeWindowCallback(int width, int height)
{
	OpenGLApplication::GetInstance()->Resize(width, height);
}

void GLUTMouseButtonCallback(int button, int state, int x, int y)
{
	OpenGLApplication::GetInstance()->MouseButton(button, state, x, y);
}

void GLUTMouseMoveCallback(int x, int y)
{
	OpenGLApplication::GetInstance()->MouseMove(x, y);
}

void GLUTKeyboardCallback(unsigned char key, int x, int y)
{
	OpenGLApplication::GetInstance()->Keyboard((int)key, x, y);
}

void GLUTSpecialKeysCallback(int key, int x, int y)
{
	OpenGLApplication::GetInstance()->Keyboard(key, x, y);
}

void ExitCallback()
{
	if (OpenGLApplication::HasStarted())
	{
		OpenGLApplication::GetInstance()->Quit();
	}
}