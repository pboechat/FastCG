#include <FastCG/Core/Exception.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/World/Component.h>
#include <FastCG/World/GameObject.h>
#include <FastCG/World/Transform.h>
#include <FastCG/World/WorldSystem.h>

#include <algorithm>
#include <cassert>

namespace FastCG
{
    GameObject::GameObject() : mpTransform(new Transform(this), [](void *pData) { delete (Transform *)pData; })
    {
    }

    GameObject::GameObject(const std::string &rName, const glm::vec3 &rScale, const glm::quat &rRotation,
                           const glm::vec3 &rPosition)
        : mName(rName),
          mpTransform(new Transform(this, rScale, rRotation, rPosition), [](void *pData) { delete (Transform *)pData; })
    {
    }

    GameObject::~GameObject() = default;

    void GameObject::SetActive(bool active)
    {
        const auto &rChildren = mpTransform->GetChildren();
        for (auto *pChild : rChildren)
        {
            pChild->GetGameObject()->SetActive(active);
        }

        mActive = active;
    }

    void GameObject::AddComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        const auto &rComponentType = pComponent->GetType();

        if (GetComponent(rComponentType) != nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Cannot add two components of the same type: %s",
                                   rComponentType.GetName().c_str());
        }

        mComponents.emplace_back(pComponent);
    }

    void GameObject::RemoveComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        auto it = std::find(mComponents.begin(), mComponents.end(), pComponent);

        assert(it != mComponents.end());

        mComponents.erase(it);
    }

    bool GameObject::HasComponent(const ComponentType &rComponentType) const
    {
        return GetComponent(rComponentType) != nullptr;
    }

    Component *GameObject::GetComponent(const ComponentType &rComponentType) const
    {
        for (auto *pComponent : mComponents)
        {
            if (pComponent->GetType().IsDerived(rComponentType))
            {
                return pComponent;
            }
        }

        return nullptr;
    }

    void GameObject::DestroyAllComponents()
    {
        auto componentsToDestroy = mComponents;
        for (auto *pComponent : componentsToDestroy)
        {
            Component::Destroy(pComponent);
        }
        componentsToDestroy.clear();

        assert(mComponents.size() == 0);
    }

    GameObject *GameObject::Instantiate(const std::string &rName, const glm::vec3 &rScale, const glm::quat &rRotation,
                                        const glm::vec3 &rPosition)
    {
        auto *pGameObject = new GameObject(rName, rScale, rRotation, rPosition);
        WorldSystem::GetInstance()->RegisterGameObject(pGameObject);
        return pGameObject;
    }

    void GameObject::Destroy(GameObject *pGameObject)
    {
        pGameObject->GetTransform()->SetParent(nullptr);
        const auto &rChildren = pGameObject->GetTransform()->GetChildren();
        while (!rChildren.empty())
        {
            Destroy(rChildren.back()->GetGameObject());
        }
        pGameObject->DestroyAllComponents();
        WorldSystem::GetInstance()->UnregisterGameObject(pGameObject);
        delete pGameObject;
    }

    AABB GameObject::GetBounds() const
    {
        AABB bounds;
        auto *pRenderable = GetComponent<Renderable>();
        if (pRenderable != nullptr)
        {
            bounds = pRenderable->GetMesh()->GetBounds();
        }
        for (auto *pChild : GetTransform()->GetChildren())
        {
            bounds.Expand(pChild->GetGameObject()->GetBounds());
        }
        return bounds;
    }

}