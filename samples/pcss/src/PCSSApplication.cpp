#include "PCSSApplication.h"
#include "Controls.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>
#include <FastCG/Camera.h>

using namespace FastCG;

namespace
{
    constexpr float GROUND_SIZE = 5;

    void LoadModel()
    {
        auto *pDefaultMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Default Material",
                                                                                 RenderingSystem::GetInstance()->FindShader("SolidColor")});

        auto *pModel = ModelImporter::Import("objs/bunny.obj", pDefaultMaterial, (ModelImporterOptionMaskType)ModelImporterOption::IS_SHADOW_CASTER);
        if (pModel == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Missing bunny model");
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
        auto *pGroundMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Ground", RenderingSystem::GetInstance()->FindShader("SolidColor")});

        auto *pGroundGameObject = GameObject::Instantiate();
        auto *pGroundMesh = StandardGeometries::CreateXZPlane("Ground", GROUND_SIZE, GROUND_SIZE);
        Renderable::Instantiate(pGroundGameObject, pGroundMaterial, pGroundMesh);
    }

    void CreateDirectionalLight()
    {
        auto *pDirectionalLightGameObject = GameObject::Instantiate();
        auto *pDirectionalLight = DirectionalLight::Instantiate(pDirectionalLightGameObject);
        pDirectionalLight->SetDiffuseColor(Colors::WHITE);
        pDirectionalLight->SetIntensity(1);

        pDirectionalLightGameObject->GetTransform()->SetPosition(glm::vec3(1, 1, 0));
    }

}

PCSSApplication::PCSSApplication() : Application({"pcss", 1024, 768, 60, RenderingPath::FORWARD, {"pcss"}})
{
}

void PCSSApplication::OnStart()
{
    auto *pMainCameraGameObject = GameObject::Instantiate();
    pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3(0, GROUND_SIZE * 0.5f, GROUND_SIZE));
    pMainCameraGameObject->GetTransform()->RotateAround(-20, glm::vec3(1, 0, 0));

    auto *pCamera = Camera::Instantiate(pMainCameraGameObject);
    pCamera->SetSSAOEnabled(true);

    auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
    pFlyController->SetWalkSpeed(5);
    pFlyController->SetTurnSpeed(0.25f);

    LoadModel();
    CreateGround();
    CreateDirectionalLight();

    auto *pGeneralBehavioursGameObject = GameObject::Instantiate();
    Controls::Instantiate(pGeneralBehavioursGameObject);
}
