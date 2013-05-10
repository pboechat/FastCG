#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <Type.h>
#include <GameObject.h>
#include <Pointer.h>
#include <Exception.h>

class Component
{
public:
	static const Type TYPE;

	Component() :
		mEnabled(true)
	{
	}

	virtual ~Component()
	{
	}

	inline virtual const Type& GetType() const
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

	friend class GameObject;

private:
	GameObject* mpGameObject;
	bool mEnabled;

};

template <class T> T* StaticCast(Component* pComponent);
template <class T> const T* StaticCast(const Component* pComponent);
template <class T> T* DynamicCast(Component* pComponent);
template <class T> const T* DynamicCast(const Component* pComponent);

template <class T>
inline T* StaticCast(Component* pComponent)
{
	return static_cast<T*>(pComponent);
}

template <class T>
inline const T* StaticCast(const Component* pComponent)
{
	return static_cast<const T*>(pComponent);
}

template <class T>
T* DynamicCast(Component* pComponent)
{
	return pComponent && pComponent->GetType().IsDerived(T::TYPE) ? static_cast<T*>(pComponent) : 0;
}

template <class T>
const T* DynamicCast(const Component* pComponent)
{
	return pComponent && pComponent->GetType().IsDerived(T::TYPE) ? static_cast<const T*>(pComponent) : 0;
}

typedef Pointer<Component> ComponentPtr;

#endif