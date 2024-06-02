#include "SSAOApplication.h"
#include "Controls.h"

#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/Core/Colors.h>
#include <FastCG/Core/Math.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Rendering/Camera.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/Rendering/ModelLoader.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/World/FlyController.h>

using namespace FastCG;

namespace
{
    void LoadModel()
    {
        auto pDefaultMaterial = std::make_shared<Material>(MaterialArgs{
            "Default Material", MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition("OpaqueSolidColor")});

        auto *pModel = ModelLoader::Load(AssetSystem::GetInstance()->Resolve("objs/armadillo.obj"), pDefaultMaterial);
        if (pModel == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't find armadillo model");
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
        pTransform->SetRotation(glm::quat(glm::vec3(0, glm::radians(180.0f), 0)));
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

SSAOApplication::SSAOApplication() : Application({"ssao", 1024, 768, 60, 3, false, false, {RenderingPath::DEFERRED}})
{
}

void SSAOApplication::OnStart()
{
    auto *pMainCameraGameObject = GameObject::Instantiate("Main Camera");
    pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3{0, 0.25f, 1.5f});

    Camera::Instantiate(pMainCameraGameObject, CameraSetupArgs{0.3f, 1000, 60, 1024 / (float)768},
                        ProjectionMode::PERSPECTIVE, true);

    auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
    pFlyController->SetMoveSpeed(5);
    pFlyController->SetTurnSpeed(0.25f);

    LoadModel();
    CreateDirectionalLight();

    auto *pGeneralBehavioursGameObject = GameObject::Instantiate("General Behaviours");
    Controls::Instantiate(pGeneralBehavioursGameObject);
}

FASTCG_MAIN(SSAOApplication)