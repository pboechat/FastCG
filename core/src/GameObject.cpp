#include <GameObject.h>
#include <Component.h>
#include <Type.h>
#include <Transform.h>
#include <Renderer.h>
#include <Application.h>
#include <Exception.h>

GameObject::GameObject() :
	mActive(true)
{
	mpTransform = new Transform();
	mpTransform->mpGameObject = this;

	Application::GetInstance()->RegisterGameObject(this);
}

GameObject::~GameObject()
{
	Application::GetInstance()->UnregisterGameObject(this);

	for (unsigned int i = 0; i < mComponents.size(); i++)
	{
		Application::GetInstance()->UnregisterComponent(mComponents[i]);
	}
}

void GameObject::AddComponent(const ComponentPtr& rComponentPtr)
{
	if (rComponentPtr == 0)
	{
		THROW_EXCEPTION(Exception, "Cannot add null component");
	}

	if (rComponentPtr->GetType().IsExactly(Transform::TYPE))
	{
		THROW_EXCEPTION(Exception, "Cannot add transform");
	}

	const Type& rComponentType = rComponentPtr->GetType();

	if (GetComponent(rComponentType) != 0)
	{
		THROW_EXCEPTION(Exception, "Cannot add two components of the same type: %s", rComponentType.GetName());
	}

	if (rComponentPtr->GetType().IsDerived(Renderer::TYPE))
	{
		mpRenderer = DynamicCast<Renderer>(rComponentPtr);
	}

	rComponentPtr->mpGameObject = this;
	Application::GetInstance()->RegisterComponent(rComponentPtr);
	mComponents.push_back(rComponentPtr);
}

ComponentPtr GameObject::GetComponent(const Type& rComponentType) const
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
