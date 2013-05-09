#include <Application.h>
#include <Exception.h>
#include <OpenGLExceptions.h>
#include <ShaderRegistry.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <iostream>
#include <sstream>

Application* Application::s_mpInstance = NULL;
bool Application::s_mStarted = false;

Application::Application(const std::string& rWindowTitle, int screenWidth, int screenHeight) :
	mpInput(0),
	mWindowTitle(rWindowTitle),
	mScreenWidth(screenWidth),
	mScreenHeight(screenHeight),
	mHalfScreenWidth(screenWidth / 2.0f),
	mHalfScreenHeight(screenHeight / 2.0f),
	mClearColor(0.0f, 0.0f, 0.0f, 0.0f),
	mGlobalAmbientLight(0.3f, 0.3f, 0.3f, 1.0f),
	mGLUTWindowHandle(0),
	mShowFPS(false),
	mElapsedFrames(0),
	mElapsedTime(0)
{
	s_mpInstance = this;

	mMainCameraPtr = new Camera();
}

Application::~Application()
{
	if (HasStarted())
	{
		Quit();
	}
}

void Application::RemoveUpdateable(UpdateablePtr updateablePtr)
{
	std::vector<UpdateablePtr>::iterator updateableIterator = std::find(mUpdateables.begin(), mUpdateables.end(), updateablePtr);

	if (updateableIterator == mUpdateables.end())
	{
		THROW_EXCEPTION(Exception, "Updateable not found");
	}

	mUpdateables.erase(updateableIterator);
}

void Application::RemoveDrawable(DrawablePtr drawablePtr)
{
	std::vector<DrawablePtr>::iterator drawableIterator = std::find(mDrawables.begin(), mDrawables.end(), drawablePtr);

	if (drawableIterator == mDrawables.end())
	{
		THROW_EXCEPTION(Exception, "Drawable not found");
	}

	mDrawables.erase(drawableIterator);
}

void Application::PrintUsage()
{
}

void Application::Run(int argc, char** argv)
{
	atexit(ExitCallback);

	if (!ParseCommandLineArguments(argc, argv))
	{
		PrintUsage();
		return;
	}

	SetUpGLUT(argc, argv);
	SetUpOpenGL();

	mpInput = new Input();

	try
	{
#ifdef USE_PROGRAMMABLE_PIPELINE
		ShaderRegistry::LoadShadersFromDisk("shaders");
		FontRegistry::LoadFontsFromDisk("fonts");

		mStandardFontPtr = FontRegistry::Find("verdana");
#endif

		mStartTimer.Start();
		OnStart();
		mUpdateTimer.Start();
		glutMainLoop();
		s_mStarted = true;
	}
	catch (Exception& e) 
	{
		std::cerr << "Fatal Exception: " << e.GetFullDescription() << std::endl;
	}
}

bool Application::ParseCommandLineArguments(int argc, char** argv)
{
	return true;
}

void Application::SetUpGLUT(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(mScreenWidth, mScreenHeight);
	mGLUTWindowHandle = glutCreateWindow(mWindowTitle.c_str());
	glewInit();
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutIgnoreKeyRepeat(1);
	glutIdleFunc(GLUTIdleCallback);
	glutDisplayFunc(GLUTDisplayCallback);
	glutReshapeFunc(GLUTReshapeWindowCallback);
	glutMouseFunc(GLUTMouseButtonCallback);
	glutMouseWheelFunc(GLUTMouseWheelCallback);
	glutMotionFunc(GLUTMouseMoveCallback);
	glutPassiveMotionFunc(GLUTMouseMoveCallback);
	glutKeyboardFunc(GLUTKeyboardCallback);
	glutKeyboardUpFunc(GLUTKeyboardUpCallback);
	glutSpecialFunc(GLUTSpecialKeysCallback);
	glutSpecialUpFunc(GLUTSpecialKeysUpCallback);
}

void Application::SetUpOpenGL()
{
	glEnable(GL_DEPTH_TEST);

#ifdef USE_PROGRAMMABLE_PIPELINE
	glEnable(GL_PROGRAM_POINT_SIZE);
#else
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
#endif
}

void Application::Update()
{
	BeforeUpdate();

	mpInput->Swap();

	mUpdateTimer.End();

	for (unsigned int i = 0; i < mUpdateables.size(); i++)
	{
		mUpdateables[i]->Update((float)mStartTimer.GetTime(), (float)mUpdateTimer.GetElapsedTime());
	}

	mUpdateTimer.Start();

	mFrameTimer.Start();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);

#ifndef USE_PROGRAMMABLE_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&mMainCameraPtr->GetProjection()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mMainCameraPtr->GetView()[0][0]);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &mGlobalAmbientLight[0]);

	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		glEnable(GL_LIGHT0 + i);

		LightPtr light = mLights[i];

		glm::vec4 lightPosition = glm::vec4(light->GetPosition(), 1.0f);

		// TODO: GL_LIGHT0 + i might be a dangereous trick!
		glLightfv(GL_LIGHT0 + i, GL_POSITION, &lightPosition[0]);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, &light->GetAmbientColor()[0]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &light->GetDiffuseColor()[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &light->GetSpecularColor()[0]);
	}
#endif

	for (unsigned int i = 0; i < mDrawables.size(); i++)
	{
		mDrawables[i]->Draw();
	}

	mFrameTimer.End();

	mElapsedTime += mFrameTimer.GetElapsedTime();
	mElapsedFrames++;

	if (mShowFPS)
	{
		char text[128];
		sprintf(text, "FPS: %.3f", mElapsedFrames / mElapsedTime);
#ifdef USE_PROGRAMMABLE_PIPELINE
		DrawText(text, FontRegistry::STANDARD_FONT_SIZE, mScreenWidth - 144, FontRegistry::STANDARD_FONT_SIZE, mStandardFontPtr, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#else
		DrawText(text, 16, mScreenWidth - 144, 16, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#endif
	}

	DrawAllTexts();

	glutSwapBuffers();

	AfterUpdate();
}

void Application::Resize(int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	mHalfScreenWidth = width * 0.5f;
	mHalfScreenHeight = height * 0.5f;
	SetUpViewport();
	OnResize();
}

void Application::OnStart()
{
}

void Application::BeforeUpdate()
{
}

void Application::AfterUpdate()
{
}

void Application::OnResize()
{
}

void Application::OnFinish()
{
}

void Application::SetUpViewport()
{
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}

void Application::Quit()
{
	if (mpInput != 0)
	{
		delete mpInput;
	}

	s_mStarted = false;
	OnFinish();
	mStartTimer.End();
	mDrawables.clear();
	glutDestroyWindow(mGLUTWindowHandle);
}

bool Application::HasStarted()
{
	return s_mStarted;
}

void Application::MouseButton(int button, int state, int x, int y)
{
	mpInput->SetMouseButton(button, (state == MouseButton::PRESSED));
	OnMouseButton(button, state, x, y);
}

void Application::MouseWheel(int button, int direction, int x, int y)
{
	if (direction == 1)
	{
		mpInput->IncrementMouseWheelDelta();
	}
	else if (direction == -1)
	{
		mpInput->DecrementMouseWheelDelta();
	}

	OnMouseWheel(button, direction, x, y);
}

void Application::MouseMove(int x, int y)
{
	mpInput->SetMousePosition(glm::vec2((float)x, (float)y));
	OnMouseMove(x, y);
}

void Application::Keyboard(int keyCode, int x, int y, bool state)
{
	mpInput->SetKey(keyCode, state);

	if (state)
	{
		OnKeyPress(keyCode);
	}
	else
	{
		OnKeyRelease(keyCode);
	}
}

void Application::OnMouseButton(int button, int state, int x, int y)
{
}

void Application::OnMouseWheel(int button, int direction, int x, int y)
{
}

void Application::OnMouseMove(int x, int y)
{
}

void Application::OnKeyPress(int keyCode)
{
}

void Application::OnKeyRelease(int keyCode)
{
}

void Application::DrawAllTexts()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifndef USE_PROGRAMMABLE_PIPELINE
	glDisable(GL_LIGHTING);

	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(&glm::ortho(0.0f, (float)mScreenWidth, 0.0f, (float)mScreenHeight)[0][0]);
	glPopAttrib();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))[0][0]);
#endif

	for (unsigned int i = 0; i < mDrawTextRequests.size(); i++)
	{
		DrawTextRequest& rDrawTextRequest = mDrawTextRequests[i];
#ifndef USE_PROGRAMMABLE_PIPELINE
		glColor4fv(&rDrawTextRequest.color[0]);
		glRasterPos2i(rDrawTextRequest.x, (mScreenHeight - rDrawTextRequest.y));
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)rDrawTextRequest.text.c_str());		
#else
		rDrawTextRequest.fontPtr->DrawText(rDrawTextRequest.text, rDrawTextRequest.size, rDrawTextRequest.x, (mScreenHeight - rDrawTextRequest.y), rDrawTextRequest.color);
#endif
	}

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();

	glEnable(GL_LIGHTING);
#endif
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	// remove all draw text requests
	mDrawTextRequests.clear();
}

#ifdef USE_PROGRAMMABLE_PIPELINE
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, FontPtr fontPtr, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(DrawTextRequest(rText, size, x, y, fontPtr, rColor));
}
#else
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(DrawTextRequest(rText, size, x, y, rColor));
}
#endif

void GLUTIdleCallback()
{
	Application::GetInstance()->Update();
}

void GLUTDisplayCallback()
{
	Application::GetInstance()->Update();
}

void GLUTReshapeWindowCallback(int width, int height)
{
	Application::GetInstance()->Resize(width, height);
}

void GLUTMouseButtonCallback(int button, int state, int x, int y)
{
	Application::GetInstance()->MouseButton(button, state, x, y);
}

void GLUTMouseWheelCallback(int button, int direction, int x, int y)
{
	Application::GetInstance()->MouseWheel(button, direction, x, y);
}

void GLUTMouseMoveCallback(int x, int y)
{
	Application::GetInstance()->MouseMove(x, y);
}

void GLUTKeyboardCallback(unsigned char keyCode, int x, int y)
{
	Application::GetInstance()->Keyboard((int) keyCode, x, y, true);
}

void GLUTKeyboardUpCallback(unsigned char keyCode, int x, int y)
{
	Application::GetInstance()->Keyboard((int) keyCode, x, y, false);
}

void GLUTSpecialKeysCallback(int keyCode, int x, int y)
{
	Application::GetInstance()->Keyboard(KeyCode::ToRegularKeyCode(keyCode), x, y, true);
}

void GLUTSpecialKeysUpCallback(int keyCode, int x, int y)
{
	Application::GetInstance()->Keyboard(KeyCode::ToRegularKeyCode(keyCode), x, y, false);
}

void ExitCallback()
{
	if (Application::HasStarted())
	{
		Application::GetInstance()->Quit();
	}
}
