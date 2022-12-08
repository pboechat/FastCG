#include <FastCG/WorldSystem.h>
#include <FastCG/RenderBatchStrategy.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/GameObject.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/DebugMenuSystem.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>

#include <imgui.h>

#include <unordered_set>
#include <cassert>
#include <algorithm>

#define FASTCG_REGISTER_COMPONENT(className, component)                    \
    if (component->GetType().IsDerived(className::TYPE))                   \
    {                                                                      \
        m##className##s.emplace_back(static_cast<className *>(component)); \
    }

#define FASTCG_UNREGISTER_COMPONENT(className, component)                               \
    if (component->GetType().IsDerived(className::TYPE))                                \
    {                                                                                   \
        auto it = std::find(m##className##s.begin(), m##className##s.end(), component); \
        if (it == m##className##s.end())                                                \
        {                                                                               \
            FASTCG_THROW_EXCEPTION(Exception, "Error unregistering: %s", #className);   \
        }                                                                               \
        m##className##s.erase(it);                                                      \
    }

namespace FastCG
{
    WorldSystem::~WorldSystem()
    {
        auto gameObjectsToDestroy = mGameObjects;
        for (auto *pGameObject : gameObjectsToDestroy)
        {
            GameObject::Destroy(pGameObject);
        }

        mpMainCamera = nullptr;

        assert(mGameObjects.empty());
        assert(mCameras.empty());
        assert(mDirectionalLights.empty());
        assert(mPointLights.empty());
        assert(mRenderables.empty());
        assert(mBehaviours.empty());
        assert(mComponents.empty());
    }

    void WorldSystem::Initialize()
    {
#ifdef _DEBUG
        DebugMenuSystem::GetInstance()->AddCallback("World System", std::bind(&WorldSystem::DebugMenuCallback, this, std::placeholders::_1));
        DebugMenuSystem::GetInstance()->AddItem("World System", std::bind(&WorldSystem::DebugMenuItemCallback, this, std::placeholders::_1));
#endif
    }

#ifdef _DEBUG
    void CreateObjectHierarchy(const GameObject *pGameObject, std::unordered_set<const GameObject *> &rVisitedGameObjects)
    {
        auto it = rVisitedGameObjects.find(pGameObject);
        if (it != rVisitedGameObjects.end())
        {
            return;
        }

        rVisitedGameObjects.emplace(pGameObject);

        ImGui::PushID(pGameObject);
        if (ImGui::TreeNodeEx(pGameObject->GetName().c_str(), ImGuiTreeNodeFlags_SpanFullWidth))
        {
            for (auto *pChild : pGameObject->GetTransform()->GetChildren())
            {
                CreateObjectHierarchy(pChild->GetGameObject(), rVisitedGameObjects);
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void WorldSystem::DebugMenuCallback(int result)
    {
        if (result == 1)
        {
            ImGui::OpenPopup("Object Hierarchy");
        }
        else if (result == 2)
        {
            ImGui::OpenPopup("Object Details");
        }

        if (ImGui::BeginPopup("Object Hierarchy"))
        {
            ImGui::Text("Object Hierarchy");
            std::unordered_set<const GameObject *> visitedGameObjects;
            for (auto *pGameObject : mGameObjects)
            {
                CreateObjectHierarchy(pGameObject, visitedGameObjects);
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Object Details"))
        {
            // TODO:
        }
    }

    void WorldSystem::DebugMenuItemCallback(int &result)
    {
        if (ImGui::MenuItem("Object Hierarchy"))
        {
            result = 1;
        }
        if (ImGui::MenuItem("Object Details"))
        {
            result = 2;
        }
    }
#endif

    void WorldSystem::RegisterGameObject(GameObject *pGameObject)
    {
        assert(pGameObject != nullptr);
        mGameObjects.emplace_back(pGameObject);
    }

    void WorldSystem::UnregisterGameObject(GameObject *pGameObject)
    {
        assert(pGameObject != nullptr);
        auto it = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);
        assert(it != mGameObjects.end());
        mGameObjects.erase(it);
    }

    void WorldSystem::RegisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_REGISTER_COMPONENT(DirectionalLight, pComponent);
        FASTCG_REGISTER_COMPONENT(PointLight, pComponent);
        FASTCG_REGISTER_COMPONENT(Renderable, pComponent);
        FASTCG_REGISTER_COMPONENT(Behaviour, pComponent);

        if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
        {
            RegisterCamera(static_cast<Camera *>(pComponent));
        }
        else if (pComponent->GetType().IsExactly(Renderable::TYPE))
        {
            mArgs.pRenderBatchStrategy->AddRenderable(static_cast<Renderable *>(pComponent));
        }

        mComponents.emplace_back(pComponent);
    }

    void WorldSystem::RegisterCamera(Camera *pCamera)
    {
        mCameras.emplace_back(pCamera);
        SetMainCamera(pCamera);
    }

    void WorldSystem::UnregisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_UNREGISTER_COMPONENT(Camera, pComponent);
        FASTCG_UNREGISTER_COMPONENT(DirectionalLight, pComponent);
        FASTCG_UNREGISTER_COMPONENT(PointLight, pComponent);
        FASTCG_UNREGISTER_COMPONENT(Renderable, pComponent);
        FASTCG_UNREGISTER_COMPONENT(Behaviour, pComponent);
        FASTCG_UNREGISTER_COMPONENT(Component, pComponent);

        if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
        {
            if (mCameras.size() > 1)
            {
                SetMainCamera(mCameras[0]);
            }
        }
        else if (pComponent->GetType().IsExactly(Renderable::TYPE))
        {
            mArgs.pRenderBatchStrategy->RemoveRenderable(static_cast<Renderable *>(pComponent));
        }
    }

    void WorldSystem::SetMainCamera(Camera *pCamera)
    {
        if (pCamera != nullptr)
        {
            pCamera->SetEnabled(true);
        }

        for (auto *pOtherCamera : mCameras)
        {
            if (pOtherCamera == pCamera)
            {
                continue;
            }

            pOtherCamera->SetEnabled(false);
        }

        mpMainCamera = pCamera;
    }

    void WorldSystem::Update(float cpuStart, float frameDeltaTime)
    {
        for (auto *pGameObject : mGameObjects)
        {
            if (pGameObject->GetTransform()->GetParent() != nullptr)
            {
                continue;
            }
            pGameObject->GetTransform()->Update();
        }

        for (auto *pBehaviour : mBehaviours)
        {
            pBehaviour->Update((float)cpuStart, (float)frameDeltaTime);
        }
    }
}