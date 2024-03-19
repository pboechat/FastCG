#include "PCSSApplication.h"
#include "Controls.h"

#include <FastCG/World/FlyController.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/ModelLoader.h>
#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Rendering/Camera.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Core/Math.h>
#include <FastCG/Core/Colors.h>
#include <FastCG/Assets/AssetSystem.h>

#include <memory>

using namespace FastCG;

namespace
{
    constexpr float GROUND_SIZE = 5;

    void LoadModel()
    {
        auto pDefaultMaterial = std::make_shared<Material>(MaterialArgs{"Default Material", MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition("OpaqueSolidColor")});

        auto *pModel = ModelLoader::Load(AssetSystem::GetInstance()->Resolve("objs/bunny.obj"), pDefaultMaterial, (ModelLoaderOptionMaskType)ModelLoaderOption::IS_SHADOW_CASTER);
        if (pModel == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't find bunny model");
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
        pTransform->SetPosition(glm::vec3(-center.x * scale, center.y * scale, -center.z * scale));
    }

    void CreateGround()
    {
        auto pGroundMaterial = std::make_unique<Material>(MaterialArgs{"Ground", MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition("OpaqueSolidColor")});

        auto *pGroundGameObject = GameObject::Instantiate("Ground");
        auto pGroundMesh = StandardGeometries::CreateXZPlane("Ground", GROUND_SIZE, GROUND_SIZE);
        Renderable::Instantiate(pGroundGameObject, std::move(pGroundMaterial), std::move(pGroundMesh));
    }

    void CreateDirectionalLight()
    {
        auto *pDirectionalLightGameObject = GameObject::Instantiate("Directional Light");
        auto *pDirectionalLight = DirectionalLight::Instantiate(pDirectionalLightGameObject);
        pDirectionalLight->SetDiffuseColor(Colors::WHITE);
        pDirectionalLight->SetIntensity(1);

        pDirectionalLightGameObject->GetTransform()->SetPosition(glm::vec3(1, 1, 0));
    }

}

PCSSApplication::PCSSApplication() : Application({"pcss", 1024, 768, 60, 3, false, {RenderingPath::FORWARD}})
{
}

void PCSSApplication::OnStart()
{
    auto *pMainCameraGameObject = GameObject::Instantiate("Main Camera");
    pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3(0, GROUND_SIZE * 0.5f, GROUND_SIZE));
    pMainCameraGameObject->GetTransform()->RotateAround(-20, glm::vec3(1, 0, 0));

    Camera::Instantiate(pMainCameraGameObject, CameraSetupArgs{0.3f, 1000, 60, 1024 / (float)768}, ProjectionMode::PERSPECTIVE);

    auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
    pFlyController->SetMoveSpeed(5);
    pFlyController->SetTurnSpeed(0.25f);

    LoadModel();
    CreateGround();
    CreateDirectionalLight();

    auto *pGeneralBehavioursGameObject = GameObject::Instantiate("General Behaviours");
    Controls::Instantiate(pGeneralBehavioursGameObject);
}

FASTCG_MAIN(PCSSApplication)