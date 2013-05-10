#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <Pointer.h>

#include <vector>

class Type;
class Component;
class Transform;
class Renderer;

class GameObject
{
public:
	GameObject();
	~GameObject();

	inline Transform* GetTransform()
	{
		return mpTransform;
	}

	inline const Transform* GetTransform() const
	{
		return mpTransform;
	}

	inline Renderer* GetRenderer()
	{
		return mpRenderer;
	}

	inline const Renderer* GetRenderer() const
	{
		return mpRenderer;
	}

	inline bool IsActive() const 
	{
		return mActive; 
	}

	void SetActive(bool active) 
	{ 
		mActive = active; 
	}

	void AddComponent(const Pointer<Component>& rComponentPtr);
	Pointer<Component> GetComponent(const Type& rComponentType) const;

private:
	Transform* mpTransform;
	Renderer* mpRenderer;
	std::vector<Component*> mComponents;
	bool mActive;
	
};

typedef Pointer<GameObject> GameObjectPtr;

#endif