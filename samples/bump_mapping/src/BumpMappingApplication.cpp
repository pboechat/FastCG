#include "BumpMappingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/WorldSystem.h>
#include <FastCG/WorldLoader.h>
#include <FastCG/TextureLoader.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelLoader.h>
#include <FastCG/GraphicsSystem.h>
#include <FastCG/FlyController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/ComponentRegistry.h>
#include <FastCG/Colors.h>
#include <FastCG/Camera.h>
#include <FastCG/AssetSystem.h>

using namespace FastCG;

namespace
{
	void CreateGround()
	{
		auto *pGround = GameObject::Instantiate("Ground");

		auto *pGroundMesh = StandardGeometries::CreateXZPlane("Ground", 5, 5);

		auto *pGroundColorMap = TextureLoader::Load(AssetSystem::GetInstance()->Resolve("textures/ground_d.png"));
		auto *pGroupBumpMap = TextureLoader::Load(AssetSystem::GetInstance()->Resolve("textures/ground_n.png"));

		auto *pGroundMaterial = GraphicsSystem::GetInstance()->CreateMaterial({"Ground", GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueBumpedSpecular")});
		pGroundMaterial->SetTexture("uColorMap", pGroundColorMap);
		pGroundMaterial->SetConstant("uColorMapTiling", glm::vec2(4, 4));
		pGroundMaterial->SetTexture("uBumpMap", pGroupBumpMap);
		pGroundMaterial->SetConstant("uBumpMapTiling", glm::vec2(4, 4));
		pGroundMaterial->SetConstant("uDiffuseColor", Colors::WHITE);
		pGroundMaterial->SetConstant("uSpecularColor", Colors::WHITE);
		pGroundMaterial->SetConstant("uShininess", 30);

		Renderable::Instantiate(pGround, pGroundMaterial, pGroundMesh);
	}

	void LoadModel()
	{
		auto *pMissingMaterial = GraphicsSystem::GetInstance()->CreateMaterial({"Missing Material", GraphicsSystem::GetInstance()->FindMaterialDefinition("OpaqueSolidColor")});
		pMissingMaterial->SetConstant("uDiffuseColor", Colors::PURPLE);

		auto *pModel = ModelLoader::Load(AssetSystem::GetInstance()->Resolve("objs/doomsday.obj"), pMissingMaterial);
		if (pModel == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Missing doomsday model");
		}

		const auto &bounds = pModel->GetBounds();
		auto *pTransform = pModel->GetTransform();
		float scale = 0;
		scale = bounds.max.x - bounds.min.x;
		scale = MathF::Max(bounds.max.y - bounds.min.y, scale);
		scale = MathF::Max(bounds.max.z - bounds.min.z, scale);
		scale = 1.0f / scale;

		pTransform->SetScale(glm::vec3(scale, scale, scale));
		auto center = bounds.getCenter();
		pTransform->SetPosition(glm::vec3(-center.x * scale, 0, -center.z * scale));
	}

}

BumpMappingApplication::BumpMappingApplication() : Application({"bump_mapping", 1024, 768, 60, false, {RenderingPath::FORWARD}, {{"bump_mapping"}}})
{
	ComponentRegistry::RegisterComponent<Controls>();
	ComponentRegistry::RegisterComponent<LightsAnimator>();
}

void BumpMappingApplication::OnStart()
{
	WorldLoader::Load(AssetSystem::GetInstance()->Resolve("worlds/default.world"));

	LoadModel();
	CreateGround();
}
