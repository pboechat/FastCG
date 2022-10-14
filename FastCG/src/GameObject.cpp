#include <FastCG/Transform.h>
#include <FastCG/Renderer.h>
#include <FastCG/MeshRenderer.h>
#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>
#include <FastCG/Component.h>
#include <FastCG/BaseApplication.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	GameObject::GameObject()
	{
		mpTransform = new Transform(this);
	}

	GameObject::GameObject(const std::string &rName) : mName(rName)
	{
		mpTransform = new Transform(this);
	}

	GameObject::~GameObject()
	{
		delete mpTransform;
	}

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

		if (rComponentType.IsDerived(Renderer::TYPE))
		{
			mpRenderer = static_cast<Renderer *>(pComponent);
		}

		BaseApplication::GetInstance()->RegisterComponent(pComponent);

		mComponents.emplace_back(pComponent);
	}

	void GameObject::RemoveComponent(Component *pComponent)
	{
		if (pComponent->GetType().IsDerived(Renderer::TYPE))
		{
			mpRenderer = nullptr;
		}

		auto it = std::find(mComponents.begin(), mComponents.end(), pComponent);

		assert(it != mComponents.end());

		mComponents.erase(it);

		BaseApplication::GetInstance()->UnregisterComponent(pComponent);
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

		return 0;
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

	GameObject *GameObject::Instantiate()
	{
		auto *pGameObject = new GameObject();
		BaseApplication::GetInstance()->RegisterGameObject(pGameObject);
		return pGameObject;
	}

	GameObject *GameObject::Instantiate(const std::string &rName)
	{
		auto *pGameObject = new GameObject(rName);
		BaseApplication::GetInstance()->RegisterGameObject(pGameObject);
		return pGameObject;
	}

	void GameObject::Destroy(GameObject *pGameObject)
	{
		pGameObject->DestroyAllComponents();
		BaseApplication::GetInstance()->UnregisterGameObject(pGameObject);
		delete pGameObject;
	}

	AABB GameObject::GetBounds() const
	{
		AABB bounds;
		auto *pMeshRenderer = static_cast<MeshRenderer *>(GetComponent(MeshRenderer::TYPE));
		if (pMeshRenderer != nullptr)
		{
			for (const auto &pMesh : pMeshRenderer->GetMeshes())
			{
				bounds.Expand(pMesh->GetBounds());
			}
		}
		for (auto *pChild : GetTransform()->GetChildren())
		{
			bounds.Expand(pChild->GetGameObject()->GetBounds());
		}
		return bounds;
	}

}