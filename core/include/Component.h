#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <GameObject.h>
#include <Exception.h>

#include <string>

class ComponentType
{
public:
	ComponentType(const std::string& rName, const ComponentType* pBaseType) :
	  mName(rName),
	  mpBaseType(pBaseType)
	{
	}

	~ComponentType()
	{
	}

	inline const std::string& GetName() const
	{
		return mName;
	}

	inline bool IsExactly(const ComponentType& rType) const
	{
		return &rType == this;
	}

	inline const ComponentType* GetBaseType() const
	{
		return mpBaseType;
	}

	bool IsDerived(const ComponentType& rType) const
	{
		const ComponentType* pSearch = this;
		while (pSearch)
		{
			if (pSearch == &rType)
			{
				return true;
			}

			pSearch = pSearch->mpBaseType;
		}

		return false;
	}

private:
	std::string mName;
	const ComponentType* mpBaseType;

};

class Component
{
public:
	static const ComponentType TYPE;

	inline virtual const ComponentType& GetType() const
	{
		return TYPE;
	}

	inline bool IsEnabled() const
	{
		return mEnabled;
	}

	inline void SetEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	inline GameObject* GetGameObject()
	{
		return mpGameObject;
	}

	inline const GameObject* GetGameObject() const
	{
		return mpGameObject;
	}

	static void Destroy(Component* pComponent)
	{
		pComponent->OnDestroy();
		pComponent->RemoveFromParent();
		delete pComponent;
	}

protected:
	static void AddToGameObject(GameObject* pGameObject, Component* pComponent)
	{
		pGameObject->AddComponent(pComponent);
	}

	Component(GameObject* pGameObject) :
		mpGameObject(pGameObject),
		mEnabled(true)
	{
	}

	virtual ~Component()
	{
	}

	inline void RemoveFromParent()
	{
		mpGameObject->RemoveComponent(this);
	}

	virtual void OnInstantiate()
	{
	}

	virtual void OnDestroy()
	{
	}

private:
	GameObject* mpGameObject;
	bool mEnabled;

};

#define COMPONENT(className, baseClassName) \
class className : public baseClassName \
{ \
public: \
	static const ComponentType TYPE; \
	inline virtual const ComponentType& GetType() const { return TYPE; } \
	static className* Instantiate(GameObject* pGameObject)  \
	{ \
		className* pComponent = new className(pGameObject); \
		Component::AddToGameObject(pGameObject, pComponent); \
		pComponent->OnInstantiate(); \
		return pComponent; \
	} \
private: \
	className(GameObject* pGameObject) : baseClassName(pGameObject) {} \
	~className() {} \

#define ABSTRACT_COMPONENT(className, baseClassName) \
class className : public baseClassName \
{ \
public: \
	static const ComponentType TYPE; \
	inline virtual const ComponentType& GetType() const { return TYPE; } \
protected: \
	className(GameObject* pGameObject) : baseClassName(pGameObject) {} \
	~className() {} \

#define COMPONENT_IMPLEMENTATION(className, baseClassName) \
	const ComponentType className::TYPE(#className, &baseClassName::TYPE) \

#endif