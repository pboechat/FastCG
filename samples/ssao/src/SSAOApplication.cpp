#include "SSAOApplication.h"
#include "Controls.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MeshRenderer.h>
#include <FastCG/MeshFilter.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/DeferredRenderingStrategy.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>

SSAOApplication::SSAOApplication() :
	Application({ "ssao", 1024, 768, 60, RenderingPath::RP_DEFERRED_RENDERING })
{
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void LoadModel()
{
	auto* pModel = ModelImporter::Import("objs/Doomsday.obj");
	if (pModel == nullptr)
	{
		THROW_EXCEPTION(Exception, "Missing doomsday model");
	}

	const auto& bounds = pModel->GetBounds();
	auto* pTransform = pModel->GetTransform();
	float scale = 0;
	scale = bounds.max.x - bounds.min.x;
	scale = MathF::Max(bounds.max.y - bounds.min.y, scale);
	scale = MathF::Max(bounds.max.z - bounds.min.z, scale);
	scale = 1.0f / scale;

	pTransform->SetScale(glm::vec3(scale, scale, scale));
	auto center = bounds.getCenter();
	pTransform->SetPosition(glm::vec3(-center.x * scale, 0, -center.z * scale));
}

void SSAOApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 1, 3));
	GetMainCamera()->SetSSAOEnabled(true);

	auto pDeferredRenderingStrategy = std::static_pointer_cast<DeferredRenderingStrategy>(Application::GetInstance()->GetRenderingStrategy());
	pDeferredRenderingStrategy->SetSSAOBlurEnabled(true);
	pDeferredRenderingStrategy->SetSSAODistanceScale(1);
	pDeferredRenderingStrategy->SetSSAORadius(1);

	{
		auto* pGameObject = GameObject::Instantiate();
		pGameObject->GetTransform()->SetPosition(glm::vec3(1, 1, 0));

		auto* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
		pDirectionalLight->SetDiffuseColor(Colors::WHITE);
		pDirectionalLight->SetIntensity(1);
	}

	mpDefaultMaterial = std::make_shared<Material>(ShaderRegistry::Find("SolidColor"));
	mpDefaultMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpDefaultMaterial->SetVec4("specularColor", Colors::WHITE);
	mpDefaultMaterial->SetFloat("shininess", 1);

	{
		auto* pGameObject = GameObject::Instantiate();

		auto* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);
		mMeshes.emplace_back(StandardGeometries::CreateXZPlane(5, 5));
		pMeshRenderer->AddMesh(mMeshes.back());

		auto* pMeshFilter = MeshFilter::Instantiate(pGameObject);
		pMeshFilter->SetMaterial(mpDefaultMaterial);
	}

	LoadModel();
	
	{
		auto* pGameObject = GameObject::Instantiate();

		auto* pControls = Controls::Instantiate(pGameObject);

		auto* pFlyCameraController = FlyCameraController::Instantiate(pGameObject);
		pFlyCameraController->SetWalkSpeed(5.0f);
		pFlyCameraController->SetTurnSpeed(5.0f);
	}
}
