#include <Application.h>
#include <OpenGLExceptions.h>

#include <algorithm>

Application* Application::s_mpInstance = NULL;
bool Application::s_mStarted = false;

Application::Application(const std::string& rWindowTitle, int screenWidth, int screenHeight) :
	mWindowTitle(rWindowTitle),
	mScreenWidth(screenWidth),
	mScreenHeight(screenHeight),
	mHalfScreenWidth(screenWidth / 2.0f),
	mHalfScreenHeight(screenHeight / 2.0f),
	mClearColor(0.0f, 0.0f, 0.0f, 0.0f),
	mGlobalAmbientLight(0.3f, 0.3f, 0.3f, 1.0f),
	mGLUTWindowHandle(0)
{
	s_mpInstance = this;
}

Application::~Application()
{
	if (HasStarted())
	{
		Quit();
	}
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

	if (OnStart())
	{
		glutMainLoop();
		s_mStarted = true;
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
	glutDisplayFunc(GLUTDisplayCallback);
	glutIdleFunc(GLUTDisplayCallback);
	glutReshapeFunc(GLUTReshapeWindowCallback);
	glutMouseFunc(GLUTMouseButtonCallback);
	glutMouseWheelFunc(GLUTMouseWheelCallback);
	glutMotionFunc(GLUTMouseMoveCallback);
	glutPassiveMotionFunc(GLUTMouseMoveCallback);
	glutKeyboardFunc(GLUTKeyboardCallback);
	glutSpecialFunc(GLUTSpecialKeysCallback);
}

void Application::SetUpOpenGL()
{
	glEnable(GL_DEPTH_TEST);

#ifndef USE_OPENGL4
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
#endif
}

void Application::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);

#ifndef USE_OPENGL4
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&mMainCamera.GetProjection()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mMainCamera.GetView()[0][0]);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &mGlobalAmbientLight[0]);

	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		glEnable(GL_LIGHT0 + i);

		LightPtr light = mLights[i];
		// TODO: GL_LIGHT0 + i might be a dangereous trick!
		glLightfv(GL_LIGHT0 + i, GL_POSITION, &light->GetPosition()[0]);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, &light->GetAmbientColor()[0]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &light->GetDiffuseColor()[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &light->GetSpecularColor()[0]);
	}
#endif

	BeforeDisplay();

	for (unsigned int i = 0; i < mGeometries.size(); i++)
	{
		mGeometries[i]->Draw();
	}

	AfterDisplay();
	glutSwapBuffers();
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

bool Application::OnStart()
{
	return true;
}

void Application::BeforeDisplay()
{
}

void Application::AfterDisplay()
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
	s_mStarted = false;
	OnFinish();
	mGeometries.clear();
	glutDestroyWindow(mGLUTWindowHandle);
}
bool Application::HasStarted()
{
	return s_mStarted;
}

void Application::MouseButton(int button, int state, int x, int y)
{
	OnMouseButton(button, state, x, y);
}

void Application::MouseWheel(int button, int direction, int x, int y)
{
	OnMouseWheel(button, direction, x, y);
}

void Application::MouseMove(int x, int y)
{
	OnMouseMove(x, y);
}

void Application::Keyboard(int key, int x, int y)
{
	OnKeyPress(key);
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

void Application::OnKeyPress(int key)
{
}

void GLUTDisplayCallback()
{
	Application::GetInstance()->Display();
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

void GLUTKeyboardCallback(unsigned char key, int x, int y)
{
	Application::GetInstance()->Keyboard((int) key, x, y);
}

void GLUTSpecialKeysCallback(int key, int x, int y)
{
	Application::GetInstance()->Keyboard(key, x, y);
}

void ExitCallback()
{
	if (Application::HasStarted())
	{
		Application::GetInstance()->Quit();
	}
}
