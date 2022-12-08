#include <FastCG/WorldSystem.h>
#include <FastCG/Transform.h>
#include <FastCG/Renderable.h>
#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>
#include <FastCG/Component.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	GameObject::GameObject() : mpTransform(new Transform(this), [](void *pData)
										   { delete pData; })
	{
	}

	GameObject::GameObject(const std::string &rName, const glm::vec3 &rScale, const glm::quat &rRotation, const glm::vec3 &rPosition) : mName(rName),
																																		mpTransform(new Transform(this, rScale, rRotation, rPosition), [](void *pData)
																																					{ delete pData; })
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
			FASTCG_THROW_EXCEPTION(Exception, "Cannot add two components of the same type: %s", rComponentType.GetName().c_str());
		}

		if (rComponentType.IsDerived(Renderable::TYPE))
		{
			mpRenderable = static_cast<Renderable *>(pComponent);
		}

		WorldSystem::GetInstance()->RegisterComponent(pComponent);

		mComponents.emplace_back(pComponent);
	}

	void GameObject::RemoveComponent(Component *pComponent)
	{
		assert(pComponent != nullptr);

		if (pComponent->GetType().IsDerived(Renderable::TYPE))
		{
			mpRenderable = nullptr;
		}

		auto it = std::find(mComponents.begin(), mComponents.end(), pComponent);

		assert(it != mComponents.end());

		mComponents.erase(it);

		WorldSystem::GetInstance()->UnregisterComponent(pComponent);
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

	GameObject *GameObject::Instantiate(const std::string &rName, const glm::vec3 &rScale, const glm::quat &rRotation, const glm::vec3 &rPosition)
	{
		auto *pGameObject = new GameObject(rName, rScale, rRotation, rPosition);
		WorldSystem::GetInstance()->RegisterGameObject(pGameObject);
		return pGameObject;
	}

	void GameObject::Destroy(GameObject *pGameObject)
	{
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