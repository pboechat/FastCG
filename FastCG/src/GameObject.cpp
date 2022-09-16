#include <FastCG/Transform.h>
#include <FastCG/Renderer.h>
#include <FastCG/MeshRenderer.h>
#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>
#include <FastCG/Component.h>
#include <FastCG/Application.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
	GameObject::GameObject() :
		mActive(true)
	{
		mpTransform = new Transform(this);
	}

	GameObject::GameObject(const std::string& rName) :
		mActive(true),
		mName(rName)
	{
		mpTransform = new Transform(this);
	}

	GameObject::~GameObject()
	{
		delete mpTransform;
	}

	void GameObject::SetActive(bool active)
	{
		const std::vector<Transform*>& rChildren = mpTransform->GetChildren();
		for (auto* pChild : rChildren)
		{
			pChild->GetGameObject()->SetActive(active);
		}

		mActive = active;
	}

	void GameObject::AddComponent(Component* pComponent)
	{
		if (pComponent == 0)
		{
			THROW_EXCEPTION(Exception, "Cannot add null component");
		}

		const ComponentType& rComponentType = pComponent->GetType();

		if (GetComponent(rComponentType) != 0)
		{
			THROW_EXCEPTION(Exception, "Cannot add two components of the same type: %s", rComponentType.GetName().c_str());
		}

		if (rComponentType.IsDerived(Renderer::TYPE))
		{
			mpRenderer = dynamic_cast<Renderer*>(pComponent);
		}

		Application::GetInstance()->RegisterComponent(pComponent);

		mComponents.emplace_back(pComponent);
	}

	void GameObject::RemoveComponent(Component* pComponent)
	{
		if (pComponent->GetType().IsDerived(Renderer::TYPE))
		{
			mpRenderer = 0;
		}

		std::vector<Component*>::iterator it = std::find(mComponents.begin(), mComponents.end(), pComponent);

		assert(it != mComponents.end());

		mComponents.erase(it);

		Application::GetInstance()->UnregisterComponent(pComponent);
	}

	Component* GameObject::GetComponent(const ComponentType& rComponentType) const
	{
		for (auto* pComponent : mComponents)
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
		std::vector<Component*> componentsToDestroy = mComponents;
		for (auto* pComponent : componentsToDestroy)
		{
			Component::Destroy(pComponent);
		}
		componentsToDestroy.clear();

		assert(mComponents.size() == 0);
	}

	GameObject* GameObject::Instantiate()
	{
		auto* pGameObject = new GameObject();
		Application::GetInstance()->RegisterGameObject(pGameObject);
		return pGameObject;
	}

	GameObject* GameObject::Instantiate(const std::string& rName)
	{
		auto* pGameObject = new GameObject(rName);
		Application::GetInstance()->RegisterGameObject(pGameObject);
		return pGameObject;
	}

	void GameObject::Destroy(GameObject* pGameObject)
	{
		pGameObject->DestroyAllComponents();
		Application::GetInstance()->UnregisterGameObject(pGameObject);
		delete pGameObject;
	}

	AABB GameObject::GetBounds() const
	{
		AABB bounds;
		auto* pMeshRenderer = dynamic_cast<MeshRenderer*>(GetComponent(MeshRenderer::TYPE));
		if (pMeshRenderer != nullptr)
		{
			for (const auto& pMesh : pMeshRenderer->GetMeshes())
			{
				bounds.Expand(pMesh->GetBounds());
			}
		}
		for (auto* pChild : GetTransform()->GetChildren())
		{
			bounds.Expand(pChild->GetGameObject()->GetBounds());
		}
		return bounds;
	}

}