#include <GameObject.h>
#include <Component.h>
#include <Transform.h>
#include <Renderer.h>
#include <Application.h>
#include <Exception.h>

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
GameObject::GameObject() :
	mActive(true)
{
	mpTransform = new Transform(this);
}

//////////////////////////////////////////////////////////////////////////
GameObject::GameObject(const std::string& rName) :
	mActive(true),
	mName(rName)
{
	mpTransform = new Transform(this);
}

//////////////////////////////////////////////////////////////////////////
GameObject::~GameObject()
{
	delete mpTransform;
}

//////////////////////////////////////////////////////////////////////////
void GameObject::SetActive(bool active)
{ 
	const std::vector<Transform*>& rChildren = mpTransform->GetChildren();
	for (unsigned int i = 0; i < rChildren.size(); i++)
	{
		Transform* pChild = rChildren[i];
		pChild->GetGameObject()->SetActive(active);
	}

	mActive = active;
}

//////////////////////////////////////////////////////////////////////////
void GameObject::AddComponent(Component* pComponent)
{
	if (pComponent == 0)
	{
		THROW_EXCEPTION(Exception, "Cannot add null component");
	}

	const ComponentType& rComponentType = pComponent->GetType();

	if (GetComponent(rComponentType) != 0)
	{
		THROW_EXCEPTION(Exception, "Cannot add two components of the same type: %s", rComponentType.GetName());
	}

	if (rComponentType.IsDerived(Renderer::TYPE))
	{
		mpRenderer = dynamic_cast<Renderer*>(pComponent);
	}

	Application::GetInstance()->RegisterComponent(pComponent);

	mComponents.push_back(pComponent);
}

//////////////////////////////////////////////////////////////////////////
void GameObject::RemoveComponent(Component* pComponent)
{
	if (pComponent->GetType().IsDerived(Renderer::TYPE))
	{
		mpRenderer = 0;
	}

	std::vector<Component*>::iterator it = std::find(mComponents.begin(), mComponents.end(), pComponent);

	if (it == mComponents.end())
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "it == mComponents.end()");
	}

	mComponents.erase(it);

	Application::GetInstance()->UnregisterComponent(pComponent);
}

//////////////////////////////////////////////////////////////////////////
Component* GameObject::GetComponent(const ComponentType& rComponentType) const
{
	for (unsigned int i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->GetType().IsDerived(rComponentType))
		{
			return mComponents[i];
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void GameObject::DestroyAllComponents()
{
	std::vector<Component*> componentsToDestroy = mComponents;
	for (unsigned int i = 0; i < componentsToDestroy.size(); i++)
	{
		Component::Destroy(componentsToDestroy[i]);
	}
	componentsToDestroy.clear();

	if (mComponents.size() > 0)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "mComponents.size() > 0");
	}
}

//////////////////////////////////////////////////////////////////////////
GameObject* GameObject::Instantiate()
{
	GameObject* pGameObject = new GameObject();
	Application::GetInstance()->RegisterGameObject(pGameObject);
	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
GameObject* GameObject::Instantiate(const std::string& rName)
{
	GameObject* pGameObject = new GameObject(rName);
	Application::GetInstance()->RegisterGameObject(pGameObject);
	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
void GameObject::Destroy(GameObject* pGameObject)
{
	pGameObject->DestroyAllComponents();
	Application::GetInstance()->UnregisterGameObject(pGameObject);
	delete pGameObject;
}