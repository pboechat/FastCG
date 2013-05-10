#include <Application.h>
#include <Behaviour.h>
#include <ShaderRegistry.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <Exception.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <algorithm>
#include <iostream>
#include <sstream>

Application* Application::s_mpInstance = NULL;
bool Application::s_mStarted = false;

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
	mpInternalGameObject(0)
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

void Application::RegisterGameObject(const GameObjectPtr& rGameObjectPtr)
{
	if (rGameObjectPtr == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "rGameObjectPtr == 0");
	}

	mGameObjects.push_back(rGameObjectPtr);
}

void Application::UnregisterGameObject(const GameObjectPtr& rGameObjectPtr)
{
	if (rGameObjectPtr == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "rGameObjectPtr == 0");
	}

	std::vector<GameObjectPtr>::iterator gameObjectsIterator = std::find(mGameObjects.begin(), mGameObjects.end(), rGameObjectPtr);

	if (gameObjectsIterator == mGameObjects.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "gameObjectsIterator == mGameObjects.end()");
	}

	mGameObjects.erase(gameObjectsIterator);
}

#define REGISTER_COMPONENT(className, componentPtr) \
	if (componentPtr->GetType().IsDerived(className::TYPE)) \
	{ \
		m##className##s.push_back(DynamicCast<className>(componentPtr)); \
	} \
 
void Application::RegisterComponent(const ComponentPtr& rComponentPtr)
{
	if (rComponentPtr == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "rComponentPtr == 0");
	}

	REGISTER_COMPONENT(Light, rComponentPtr)
	REGISTER_COMPONENT(LineRenderer, rComponentPtr);
	REGISTER_COMPONENT(PointsRenderer, rComponentPtr);
	REGISTER_COMPONENT(Behaviour, rComponentPtr)

	if (rComponentPtr->GetType().IsExactly(Camera::TYPE) && rComponentPtr->IsEnabled())
	{
		RegisterCamera(DynamicCast<Camera>(rComponentPtr));
	}

	else if (rComponentPtr->GetType().IsExactly(MeshFilter::TYPE))
	{
		RegisterMeshFilter(DynamicCast<MeshFilter>(rComponentPtr));
	}

	mComponents.push_back(rComponentPtr);
}

void Application::RegisterCamera(Camera* pCamera)
{
	mCameras.push_back(pCamera);
	SetMainCamera(pCamera);
}

void Application::RegisterMeshFilter(MeshFilter* pMeshFilter)
{
	mMeshFilters.push_back(pMeshFilter);
	Material* pMaterial = pMeshFilter->GetMaterial();
	bool added = false;

	for (unsigned int i = 0; i < mRenderingGroups.size(); i++)
	{
		RenderingGroup& rRenderingGroup = mRenderingGroups[i];

		if (rRenderingGroup.pMaterial == pMaterial)
		{
			rRenderingGroup.meshFilters.push_back(pMeshFilter);
			added = true;
			break;
		}
	}

	if (added)
	{
		return;
	}

	RenderingGroup newRenderingGroup;
	newRenderingGroup.pMaterial = pMaterial;
	newRenderingGroup.meshFilters.push_back(pMeshFilter);
	mRenderingGroups.push_back(newRenderingGroup);
}

#define UNREGISTER_COMPONENT(className, componentPtr) \
	if (componentPtr->GetType().IsDerived(className::TYPE)) \
	{ \
	std::vector<className##*>::iterator it = std::find(m##className##s.begin(), m##className##s.end(), componentPtr); \
	if (it == m##className##s.end()) \
		{ \
			THROW_EXCEPTION(Exception, ""); \
		} \
		m##className##s.erase(it); \
	} \
 
void Application::UnregisterComponent(const ComponentPtr& rComponentPtr)
{
	if (rComponentPtr == 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "rComponentPtr == 0");
	}

	UNREGISTER_COMPONENT(Camera, rComponentPtr)
	UNREGISTER_COMPONENT(Light, rComponentPtr)
	UNREGISTER_COMPONENT(LineRenderer, rComponentPtr)
	UNREGISTER_COMPONENT(PointsRenderer, rComponentPtr)
	UNREGISTER_COMPONENT(MeshFilter, rComponentPtr)
	UNREGISTER_COMPONENT(Behaviour, rComponentPtr)
	UNREGISTER_COMPONENT(Component, rComponentPtr)

	if (rComponentPtr->GetType().IsExactly(Camera::TYPE) && rComponentPtr->IsEnabled())
	{
		if (mCameras.size() < 1)
		{
			SetMainCamera(mCameras[0]);
		}
	}

	else if (rComponentPtr->GetType().IsExactly(MeshFilter::TYPE))
	{
		RemoveFromMeshRenderingGroups(DynamicCast<MeshFilter>(rComponentPtr));
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
	// FIXME:
	mpInternalGameObject = new GameObject();
	Camera* pCamera = new Camera();
	mpInternalGameObject->AddComponent(pCamera);
	pCamera->SetUp();

	try
	{
#ifdef USE_PROGRAMMABLE_PIPELINE
		ShaderRegistry::LoadShadersFromDisk("shaders");
		mLineStripShaderPtr = ShaderRegistry::Find("LineStrip");
		mPointsShaderPtr = ShaderRegistry::Find("Points");

		FontRegistry::LoadFontsFromDisk("fonts");
		mStandardFontPtr = FontRegistry::Find("verdana");
#endif
		mStartTimer.Start();
		OnStart();
		mUpdateTimer.Start();
		glutMainLoop();
	}

	catch (Exception& e)
	{
		std::cerr << "Fatal Exception: " << e.GetFullDescription() << std::endl;
	}

	s_mStarted = true;
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
	mpInput->Swap();
	mUpdateTimer.End();

	for (unsigned int i = 0; i < mBehaviours.size(); i++)
	{
		mBehaviours[i]->Update((float)mStartTimer.GetTime(), (float)mUpdateTimer.GetElapsedTime());
	}

	mUpdateTimer.Start();
	mFrameTimer.Start();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);

	glm::mat4 view = mpMainCamera->GetView();
	glm::mat4 projection = mpMainCamera->GetProjection();

#ifndef USE_PROGRAMMABLE_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&projection[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&view[0][0]);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &mGlobalAmbientLight[0]);

	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		glEnable(GL_LIGHT0 + i);
		LightPtr lightPtr = mLights[i];
		glm::vec4 lightPosition = glm::vec4(lightPtr->GetGameObject()->GetTransform()->GetPosition(), 1.0f);
		// TODO: GL_LIGHT0 + i might be a dangereous trick!
		glLightfv(GL_LIGHT0 + i, GL_POSITION, &lightPosition[0]);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, &lightPtr->GetAmbientColor()[0]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &lightPtr->GetDiffuseColor()[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &lightPtr->GetSpecularColor()[0]);
	}

#endif
	for (unsigned int i = 0; i < mRenderingGroups.size(); i++)
	{
		RenderingGroup& rRenderingGroup = mRenderingGroups[i];
		Material* pMaterial = rRenderingGroup.pMaterial;
		std::vector<MeshFilter*>& rMeshFilters = rRenderingGroup.meshFilters;
#ifdef USE_PROGRAMMABLE_PIPELINE
		Shader* pShader = pMaterial->GetShader();
		pShader->Bind();
		SetUpShaderLights(pShader);
#endif
		pMaterial->SetUpShaderParameters();

		for (unsigned int j = 0; j < rMeshFilters.size(); j++)
		{
			MeshFilter* pMeshFilter = rMeshFilters[j];

			if (!pMeshFilter->GetGameObject()->IsActive())
			{
				continue;
			}

			Renderer* pRenderer = pMeshFilter->GetGameObject()->GetRenderer();

			if (pRenderer == 0)
			{
				continue;
			}

			const glm::mat4& rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
#ifdef USE_PROGRAMMABLE_PIPELINE
			glm::mat4 modelView = view * rModel;
			pShader->SetMat4("_Model", rModel);
			pShader->SetMat4("_View", view);
			pShader->SetMat4("_ModelView", modelView);
			pShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			pShader->SetMat4("_Projection", projection);
			pShader->SetMat4("_ModelViewProjection", projection * modelView);
			pShader->SetVec4("_GlobalLightAmbientColor", mGlobalAmbientLight);
#else
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf(&rModel[0][0]);
#endif
			pRenderer->Render();
#ifndef USE_PROGRAMMABLE_PIPELINE
			glPopMatrix();
#endif
		}

#ifdef USE_PROGRAMMABLE_PIPELINE
		pShader->Unbind();
#endif
		// FIXME: shouldn't be necessary if we could guarantee that all textures are unbound after use!
		glBindTexture(GL_TEXTURE_2D, 0);
	}

#ifdef USE_PROGRAMMABLE_PIPELINE
	mLineStripShaderPtr->Bind();
#endif

	for (unsigned int i = 0; i < mLineRenderers.size(); i++)
	{
		LineRenderer* pLineRenderer = mLineRenderers[i];

		if (!pLineRenderer->GetGameObject()->IsActive())
		{
			continue;
		}

		const glm::mat4& rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
#ifdef USE_PROGRAMMABLE_PIPELINE
		glm::mat4 modelView = view * rModel;
		mLineStripShaderPtr->SetMat4("_Model", rModel);
		mLineStripShaderPtr->SetMat4("_View", view);
		mLineStripShaderPtr->SetMat4("_ModelView", modelView);
		mLineStripShaderPtr->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
		mLineStripShaderPtr->SetMat4("_Projection", projection);
		mLineStripShaderPtr->SetMat4("_ModelViewProjection", projection * modelView);
		mLineStripShaderPtr->SetVec4("_GlobalLightAmbientColor", mGlobalAmbientLight);
#else
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(&rModel[0][0]);
#endif
		pLineRenderer->Render();
#ifndef USE_PROGRAMMABLE_PIPELINE
		glPopMatrix();
#endif
	}

#ifdef USE_PROGRAMMABLE_PIPELINE
	mLineStripShaderPtr->Unbind();
#endif

#ifdef USE_PROGRAMMABLE_PIPELINE
	mPointsShaderPtr->Bind();
#endif

	for (unsigned int i = 0; i < mPointsRenderers.size(); i++)
	{
		PointsRenderer* pPointsRenderer = mPointsRenderers[i];

		if (!pPointsRenderer->GetGameObject()->IsActive())
		{
			continue;
		}

		const glm::mat4& rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
#ifdef USE_PROGRAMMABLE_PIPELINE
		glm::mat4 modelView = view * rModel;
		mPointsShaderPtr->SetMat4("_Model", rModel);
		mPointsShaderPtr->SetMat4("_View", view);
		mPointsShaderPtr->SetMat4("_ModelView", modelView);
		mPointsShaderPtr->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
		mPointsShaderPtr->SetMat4("_Projection", projection);
		mPointsShaderPtr->SetMat4("_ModelViewProjection", projection * modelView);
		mPointsShaderPtr->SetVec4("_GlobalLightAmbientColor", mGlobalAmbientLight);

		Points* pPoints = pPointsRenderer->GetPoints();
		if (pPoints != 0)
		{
			mPointsShaderPtr->SetFloat("size", pPoints->GetSize());
		}
#else
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(&rModel[0][0]);
#endif
		pPointsRenderer->Render();
#ifndef USE_PROGRAMMABLE_PIPELINE
		glPopMatrix();
#endif
	}

#ifdef USE_PROGRAMMABLE_PIPELINE
	mPointsShaderPtr->Unbind();
#endif

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

void Application::OnStart()
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
	mGameObjects.clear();
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

void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(DrawTextRequest(rText, size, x, y, mStandardFontPtr, rColor));
}
#else
void Application::DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor)
{
	// add a new draw text request to be processed at the end of the frame
	mDrawTextRequests.push_back(DrawTextRequest(rText, size, x, y, rColor));
}
#endif

#ifdef USE_PROGRAMMABLE_PIPELINE
void Application::SetUpShaderLights(Shader* pShader)
{
	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		Light* pLight = mLights[i];
		glm::vec3 lightPosition = pLight->GetGameObject()->GetTransform()->GetPosition();
		glm::vec4 lightAmbientColor = pLight->GetAmbientColor();
		glm::vec4 lightDiffuseColor = pLight->GetDiffuseColor();
		glm::vec4 lightSpecularColor = pLight->GetSpecularColor();
		std::stringstream variableName;
		variableName << "_Light" << i << "Position";
		pShader->SetVec3(variableName.str(), lightPosition);
		variableName.str(std::string());
		variableName.clear();
		variableName << "_Light" << i << "AmbientColor";
		pShader->SetVec4(variableName.str(), lightAmbientColor);
		variableName.str(std::string());
		variableName.clear();
		variableName << "_Light" << i << "DiffuseColor";
		pShader->SetVec4(variableName.str(), lightDiffuseColor);
		variableName.str(std::string());
		variableName.clear();
		variableName << "_Light" << i << "SpecularColor";
		pShader->SetVec4(variableName.str(), lightSpecularColor);
	}
}
#endif

void Application::RemoveFromMeshRenderingGroups(MeshFilter* pMeshFilter)
{
	Material* pMaterial = pMeshFilter->GetMaterial();
	bool removed = false;

	for (unsigned int i = 0; i < mRenderingGroups.size(); i++)
	{
		RenderingGroup& rRenderingGroup = mRenderingGroups[i];

		if (rRenderingGroup.pMaterial == pMaterial)
		{
			std::vector<MeshFilter*>& rMeshFilters = rRenderingGroup.meshFilters;
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

void ExitCallback()
{
	if (Application::HasStarted())
	{
		Application::GetInstance()->Quit();
	}
}
