#include "SSAOApplication.h"
#include "KeyBindings.h"
#include "LightsAnimator.h"

#include <DirectionalLight.h>
#include <PointLight.h>
#include <ShaderRegistry.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <PointLight.h>
#include <ModelImporter.h>
#include <TextureImporter.h>
#include <StandardGeometries.h>
#include <Colors.h>
#include <MathT.h>

#include <vector>

SSAOApplication::SSAOApplication() :
	Application("ssao", 1024, 768, 30, true, "../../core/"),
	mpGroundMesh(0),
	mpGroundMaterial(0),
	mpGroundColorMapTexture(0),
	mpGroundBumpMapTexture(0)
{
	mGlobalAmbientLight = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

SSAOApplication::~SSAOApplication()
{
	if (mpGroundMesh != 0)
	{
		delete mpGroundMesh;
	}

	if (mpGroundMaterial != 0)
	{
		delete mpGroundMaterial;
	}

	if (mpGroundColorMapTexture != 0)
	{
		delete mpGroundColorMapTexture;
	}

	if (mpGroundBumpMapTexture != 0)
	{
		delete mpGroundBumpMapTexture;
	}
}

void SSAOApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.481f, 0.465f, 0.804f));
	mpMainCamera->GetGameObject()->GetTransform()->RotateAroundLocal(29.0f, glm::vec3(0, 1, 0));
	mpMainCamera->GetGameObject()->GetTransform()->RotateAroundLocal(-23.0f, glm::vec3(1, 0, 0));
	mpMainCamera->SetSSAOEnabled(true);

	GameObject* pLightGameObject;

	std::vector<GameObject*> sceneLights;

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, 1.3f, 1.3f));
	sceneLights.push_back(pLightGameObject);

	PointLight* pPointLight;

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::WHITE);
	pPointLight->SetSpecularColor(Colors::WHITE);

	ModelImporter::LogToConsole();

	GameObject* pModel = ModelImporter::Import("models/Doomsday.obj");

	const AABB& boundingVolume = pModel->GetBoundingVolume();
	Transform* pTransform = pModel->GetTransform();
	float scale = 0;
	scale = boundingVolume.max.x - boundingVolume.min.x;
	scale = MathF::Max(boundingVolume.max.y - boundingVolume.min.y, scale);
	scale = MathF::Max(boundingVolume.max.z - boundingVolume.min.z, scale);
	scale = 1.0f / scale;

	pTransform->ScaleLocal(glm::vec3(scale, scale, scale));
	pTransform->SetPosition(-boundingVolume.center * scale);

	GameObject* pGround = GameObject::Instantiate();
	pGround->GetTransform()->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));

	mpGroundMesh = StandardGeometries::CreateXZPlane(5, 5);
	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGround);
	pMeshRenderer->AddMesh(mpGroundMesh);

	mpGroundColorMapTexture = TextureImporter::Import("textures/GroundColorMap.png");
	mpGroundBumpMapTexture = TextureImporter::Import("textures/GroundBumpMap.png");

	mpGroundMaterial = new Material(ShaderRegistry::Find("BumpedSpecular"));
	mpGroundMaterial->SetTexture("colorMap", mpGroundColorMapTexture);
	mpGroundMaterial->SetTextureTiling("colorMap", glm::vec2(4, 4));
	mpGroundMaterial->SetTexture("bumpMap", mpGroundBumpMapTexture);
	mpGroundMaterial->SetTextureTiling("bumpMap", glm::vec2(4, 4));
	mpGroundMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpGroundMaterial->SetVec4("specularColor", Colors::WHITE);
	mpGroundMaterial->SetFloat("shininess", 5);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGround);
	pMeshFilter->SetMaterial(mpGroundMaterial);

	GameObject* pGameObject = GameObject::Instantiate();

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);

	LightsAnimator* pLightsAnimator = LightsAnimator::Instantiate(pGameObject);
	pLightsAnimator->SetSceneLights(sceneLights);
}
