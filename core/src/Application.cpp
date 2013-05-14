#include <Application.h>
#include <Behaviour.h>
#include <ShaderRegistry.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <RenderingStrategy.h>
#include <FixedFunctionRenderingStrategy.h>
#include <ForwardRenderingStrategy.h>
#include <Exception.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <algorithm>
#include <iostream>
#include <sstream>

Application* Application::s_mpInstance = NULL;

Application::Application(const std::string& rWindowTitle, int screenWidth, int screenHeight) :
	mWindowTitle(rWindowTitle),
	mScreenWidth(screenWidth),
	mScreenHeight(screenHeight),
	mHalfScreenWidth(screenWidth / 2.0f),
	mHalfScreenHeight(screenHeight / 2.0f),
	mAspectRatio(mScreenWidth / (float) mScreenHeight),
	mClearColor(0.0f, 0.0f, 0.0f, 0.0f),
	mGlobalAmbientLight(0.3f, 0.3f, 0.3f, 1.0f),
	mGLUTWindowHandle(0),
	mShowFPS(false),
	mElapsedFrames(0),
	mElapsedTime(0),
	mpInput(0),
	mpRenderingStrategy(0)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mpStandardFont = 0;
#endif
	s_mpInstance = this;
}

Application::~Application()
{
	if (mpRenderingStrategy != 0)
	{
		delete mpRenderingStrategy;
	}

	if (mpInput != 0)
	{
		delete mpInput;
	}

	mpMainCamera = 0;

	for (unsigned int i = 0; i < mRenderingGroups.size(); i++)
	{
		delete mRenderingGroups[i];
	}

	mRenderingGroups.clear();

	for (unsigned int i = 0; i < mDrawTextRequests.size(); i++)
	{
		delete mDrawTextRequests[i];
	}

	mDrawTextRequests.clear();
#ifdef USE_PROGRAMMABLE_PIPELINE
	ShaderRegistry::Unload();
	FontRegistry::Unload();
	mpStandardFont = 0;
#endif
	std::vector<GameObject*> gameObjectsToDestroy = mGameObjects;

	for (unsigned int i = 0; i < gameObjectsToDestroy.size(); i++)
	{
		GameObject::Destroy(gameObjectsToDestroy[i]);
	}

	gameObjectsToDestroy.clear();

	if (mGameObjects.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mGameObjects.size() > 0");
	}

	if (mCameras.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mCameras.size() > 0");
	}

	if (mLights.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mLights.size() > 0");
	}

	if (mMeshFilters.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mMeshFilters.size() > 0");
	}

	if (mBehaviours.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mBehaviours.size() > 0");
	}

	if (mLineRenderers.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mLineRenderers.size() > 0");
	}

	if (mPointsRenderers.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mPointsRenderers.size() > 0");
	}

	if (mComponents.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mComponents.size() > 0");
	}

	s_mpInstance = 0;
}

void Application::RegisterGameObject(GameObject* pGameObject)
{
	if (pGameObject == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pGameObject == 0");
	}

	mGameObjects.push_back(pGameObject);
}

void Application::UnregisterGameObject(GameObject* pGameObject)
{
	if (pGameObject == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pGameObject == 0");
	}

	std::vector<GameObject*>::iterator gameObjectsIterator = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);

	if (gameObjectsIterator == mGameObjects.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "gameObjectsIterator == mGameObjects.end()");
	}

	mGameObjects.erase(gameObjectsIterator);
}

#define REGISTER_COMPONENT(className, component) \
	if (component->GetType().IsDerived(className::TYPE)) \
	{ \
		m##className##s.push_back(dynamic_cast<className*>(component)); \
	} \
 
void Application::RegisterComponent(Component* pComponent)
{
	if (pComponent == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pComponent == 0");
	}

	REGISTER_COMPONENT(Light, pComponent);
	REGISTER_COMPONENT(MeshFilter, pComponent);
	REGISTER_COMPONENT(LineRenderer, pComponent);
	REGISTER_COMPONENT(PointsRenderer, pComponent);
	REGISTER_COMPONENT(Behaviour, pComponent)

	if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
	{
		RegisterCamera(dynamic_cast<Camera*>(pComponent));
	}

	mComponents.push_back(pComponent);
}

void Application::RegisterCamera(Camera* pCamera)
{
	mCameras.push_back(pCamera);
	SetMainCamera(pCamera);
}

#define UNREGISTER_COMPONENT(className, component) \
	if (component->GetType().IsDerived(className::TYPE)) \
	{ \
		std::vector<className##*>::iterator it = std::find(m##className##s.begin(), m##className##s.end(), component); \
		if (it == m##className##s.end()) \
		{ \
			THROW_EXCEPTION(Exception, "Error unregistering: %s", #className); \
		} \
		m##className##s.erase(it); \
	} \
 
void Application::UnregisterComponent(Component* pComponent)
{
	if (pComponent == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "pComponent == 0");
	}

	UNREGISTER_COMPONENT(Camera, pComponent)
	UNREGISTER_COMPONENT(Light, pComponent)
	UNREGISTER_COMPONENT(LineRenderer, pComponent)
	UNREGISTER_COMPONENT(PointsRenderer, pComponent)
	UNREGISTER_COMPONENT(MeshFilter, pComponent)
	UNREGISTER_COMPONENT(Behaviour, pComponent)
	UNREGISTER_COMPONENT(Component, pComponent)

	if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
	{
		if (mCameras.size() > 1)
		{
			SetMainCamera(mCameras[0]);
		}
	}
}

void Application::SetMainCamera(Camera* pCamera)
{
	pCamera->SetEnabled(true);

	for (unsigned int i = 0; i < mCameras.size(); i++)
	{
		Camera* pOtherCamera = mCameras[i];

		if (pOtherCamera == pCamera)
		{
			continue;
		}

		pOtherCamera->SetEnabled(false);
	}

	mpMainCamera = pCamera;
	mpMainCamera->SetAspectRatio(mAspectRatio);
}

void Application::Run(int argc, char** argv)
{
	if (!ParseCommandLineArguments(argc, argv))
	{
		PrintUsage();
		return;
	}

	SetUpGLUT(argc, argv);
	SetUpOpenGL();
	mpInput = new Input();
	mpInternalGameObject = GameObject::Instantiate();
	Camera* pCamera = Camera::Instantiate(mpInternalGameObject);
	pCamera->SetUp();

	try
	{
#ifdef USE_PROGRAMMABLE_PIPELINE
		ShaderRegistry::LoadShadersFromDisk("shaders");
		FontRegistry::LoadFontsFromDisk("fonts");
		mpStandardFont = FontRegistry::Find("verdana");
		mpRenderingStrategy = new ForwardRenderingStrategy(mLights, mGlobalAmbientLight, mRenderingGroups, mLineRenderers, mPointsRenderers);
#else
		mpRenderingStrategy = new FixedFunctionRenderingStrategy(mLights, mGlobalAmbientLight, mRenderingGroups, mLineRenderers, mPointsRenderers);
#endif
		mStartTimer.Start();
		OnStart();
		mUpdateTimer.Start();
		glutMainLoop();
		mUpdateTimer.End();
		mStartTimer.End();
		OnEnd();
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
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
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
	mpInput->Swap();
	mUpdateTimer.End();
	for (unsigned int i = 0; i < mBehaviours.size(); i++)
	{
		mBehaviours[i]->Update((float)mStartTimer.GetTime(), (float)mUpdateTimer.GetElapsedTime());
	}
	mUpdateTimer.Start();
	Render();
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
		DrawTextRequest* pDrawTextRequest = mDrawTextRequests[i];
#ifdef USE_PROGRAMMABLE_PIPELINE
		pDrawTextRequest->pFont->DrawText(pDrawTextRequest->text, pDrawTextRequest->size, pDrawTextRequest->x, (mScreenHeight - pDrawTextRequest->y), pDrawTextRequest->color);		
#else
		glColor4fv(&pDrawTextRequest->color[0]);
		glRasterPos2i(pDrawTextRequest->x, (mScreenHeight - pDrawTextRequest->y));
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)pDrawTextRequest->text.c_str());
#endif
		delete pDrawTextRequest;
	}

	mDrawTextRequests.clear();
#ifndef USE_PROGRAMMABLE_PIPELINE
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glEnable(GL_LIGHTING);
#endif
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Application::ShowFPS()
{
	static char fpsText[128];
	sprintf(fpsText, "FPS: %.3f", mElapsedFrames / mElapsedTime);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef USE_PROGRAMMABLE_PIPELINE
	mpStandardFont->DrawText(fpsText, FontRegistry::STANDARD_FONT_SIZE, mScreenWidth - 144, mScreenHeight - FontRegistry::STANDARD_FONT_SIZE, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#else
	glDisable(GL_LIGHTING);
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(&glm::ortho(0.0f, (float)mScreenWidth, 0.0f, (float)mScreenHeight)[0][0]);
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))[0][0]);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glRasterPos2i(mScreenWidth - 144, (mScreenHeight - 12));
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char*)fpsText);
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glEnable(GL_LIGHTING);
#endif
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Application::Render()
{
	mFrameTimer.Start();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);
	
	mpRenderingStrategy->Render(mpMainCamera);

	DrawAllTexts();

	mFrameTimer.End();
	mElapsedTime += mFrameTimer.GetElapsedTime();
	mElapsedFrames++;

	if (mShowFPS)
	{
		ShowFPS();
	}

	glutSwapBuffers();
}

#ifdef USE_PROGRAMMABLE_PIPELINE
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, Font* pFont, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(new DrawTextRequest(rText, size, x, y, pFont, rColor));
}

void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(new DrawTextRequest(rText, size, x, y, mpStandardFont, rColor));
}
#else
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(new DrawTextRequest(rText, size, x, y, rColor));
}
#endif

void Application::AddToMeshRenderingGroup(MeshFilter* pMeshFilter, Material* pMaterial)
{
	bool added = false;

	for (unsigned int i = 0; i < mRenderingGroups.size(); i++)
	{
		RenderingGroup* pRenderingGroup = mRenderingGroups[i];

		if (pRenderingGroup->pMaterial == pMaterial)
		{
			pRenderingGroup->meshFilters.push_back(pMeshFilter);
			added = true;
			break;
		}
	}

	if (added)
	{
		return;
	}

	RenderingGroup* pNewRenderingGroup = new RenderingGroup();
	pNewRenderingGroup->pMaterial = pMaterial;
	pNewRenderingGroup->meshFilters.push_back(pMeshFilter);
	mRenderingGroups.push_back(pNewRenderingGroup);
}

void Application::RemoveFromMeshRenderingGroup(MeshFilter* pMeshFilter, Material* pMaterial)
{
	RenderingGroup* pRenderingGroup;
	bool removed = false;

	for (unsigned int i = 0; i < mRenderingGroups.size(); i++)
	{
		pRenderingGroup = mRenderingGroups[i];

		if (pRenderingGroup->pMaterial == pMaterial)
		{
			std::vector<MeshFilter*>& rMeshFilters = pRenderingGroup->meshFilters;
			std::vector<MeshFilter*>::iterator it = std::find(rMeshFilters.begin(), rMeshFilters.end(), pMeshFilter);

			if (it == rMeshFilters.end())
			{
				// FIXME: checking invariants
				THROW_EXCEPTION(Exception, "it == rMeshFilters.end()");
			}

			rMeshFilters.erase(it);
			removed = true;
			break;
		}
	}

	if (!removed)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "!removed");
	}

	if (pRenderingGroup->meshFilters.size() == 0)
	{
		std::vector<RenderingGroup*>::iterator it = std::find(mRenderingGroups.begin(), mRenderingGroups.end(), pRenderingGroup);
		mRenderingGroups.erase(it);
		delete pRenderingGroup;
	}
}

void Application::Resize(int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	mHalfScreenWidth = width * 0.5f;
	mHalfScreenHeight = height * 0.5f;
	mAspectRatio = mScreenWidth / (float) mScreenHeight;
	mpMainCamera->SetAspectRatio(mAspectRatio);
	SetUpViewport();
	OnResize();
}

void Application::SetUpViewport()
{
	glViewport(0, 0, mScreenWidth, mScreenHeight);
}

void Application::Exit()
{
	glutLeaveMainLoop();
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
	mpInput->SetMousePosition(glm::vec2((float)x, (float)(mScreenHeight - y)));
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

void Application::PrintUsage()
{
}

void Application::OnStart()
{
}

void Application::OnResize()
{
}

void Application::OnEnd()
{
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

