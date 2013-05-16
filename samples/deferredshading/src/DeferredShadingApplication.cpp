#include "DeferredShadingApplication.h"
#include "KeyBindings.h"
#include "LightsAnimator.h"

#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <DirectionalLight.h>
#include <PointLight.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ModelImporter.h>
#include <FirstPersonCameraController.h>
#include <TextureUtils.h>
#include <MathT.h>

#include <vector>

const unsigned int DeferredShadingApplication::NUMBER_OF_LIGHTS = 1;
const float DeferredShadingApplication::LIGHT_DISTANCE = 5.0f;
const float DeferredShadingApplication::WALK_SPEED = 5.0f;
const float DeferredShadingApplication::TURN_SPEED = 100.0f;

DeferredShadingApplication::DeferredShadingApplication() :
#ifdef USE_PROGRAMMABLE_PIPELINE
	Application("deferredshading", 1024, 768, true),
#else
	Application("deferredshading", 1024, 768),
#endif
	mpTexture(0),
	mpMaterial(0),
	mpMesh(0)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void DeferredShadingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 0.0f, 10.0f));

	std::vector<Light*> lights;

	GameObject* pGameObject;

	/*float angleIncrement = MathF::TWO_PI / (float)NUMBER_OF_LIGHTS;
	float angle = 0;
	float intensity = 1.0f / NUMBER_OF_LIGHTS;
	for (unsigned int i = 0; i < NUMBER_OF_LIGHTS; i++, angle += angleIncrement)
	{
		glm::vec3 position = glm::vec3(MathF::Sin(angle), 0.0f, MathF::Cos(angle)) * LIGHT_DISTANCE;

		pGameObject = GameObject::Instantiate();
		pGameObject->GetTransform()->Translate(position);

		PointLight* pLight = PointLight::Instantiate(pGameObject);
		pLight->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		pLight->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		pLight->SetSpecularColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		pLight->SetIntensity(intensity);
		lights.push_back(pLight);
	}*/

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Translate(glm::vec3(0.0f, 0.0f, 5.0f));

	PointLight* pPointLight = PointLight::Instantiate(pGameObject);
	pPointLight->SetAmbientColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	pPointLight->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pPointLight->SetSpecularColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pPointLight->SetIntensity(1.0f);
	lights.push_back(pPointLight);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Translate(glm::vec3(0.0f, -1.0f, 0.0f));

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetAmbientColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	pDirectionalLight->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectionalLight->SetSpecularColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectionalLight->SetIntensity(1.0f);

	pGameObject = GameObject::Instantiate();

	mpMesh = StandardGeometries::CreateSphere(1.0f, 30);
	mpTexture = TextureUtils::LoadPNGAsTexture("textures/MarsColorMap.png");
#ifdef USE_PROGRAMMABLE_PIPELINE
	mpMaterial = new Material(ShaderRegistry::Find("DeferredShadingDiffuse"));
	mpMaterial->SetTexture("colorMap", mpTexture);
#else
	mpMaterial = new Material();
	mpMaterial->SetSpecularColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mpMaterial->SetShininess(5.0);
	mpMaterial->SetTexture(mpTexture);
#endif

	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);
	pMeshRenderer->AddMesh(mpMesh);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGameObject);
	pMeshFilter->SetMaterial(mpMaterial);

	Model* pModel = ModelImporter::Import("models/mark42/Mark 42.obj");
	pGameObject = pModel->GetRootGameObject();

	pGameObject = GameObject::Instantiate();
	KeyBindings::Instantiate(pGameObject);
	LightsAnimator* pLightsAnimator = LightsAnimator::Instantiate(pGameObject);
	pLightsAnimator->SetLights(lights);
	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pGameObject);
	pFirstPersonCameraController->SetWalkSpeed(WALK_SPEED);
	pFirstPersonCameraController->SetTurnSpeed(TURN_SPEED);
}

void DeferredShadingApplication::OnEnd()
{
	if (mpMesh != 0)
	{
		delete mpMesh;
	}

	if (mpMaterial != 0)
	{
		delete mpMaterial;
	}

	if (mpTexture != 0)
	{
		delete mpTexture;
	}
}
