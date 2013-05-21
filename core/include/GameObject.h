#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <AABB.h>

#include <vector>

class ComponentType;
class Component;
class Transform;
class Renderer;
class Application;

class GameObject
{
public:
	static GameObject* Instantiate();
	static void Destroy(GameObject* pGameObject);

	inline const AABB& GetBoundingVolume() const
	{
		return mBoundingVolume;
	}

	inline void SetBoundingVolume(const AABB& boundingVolume)
	{
		mBoundingVolume = boundingVolume;
	}

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

	Component* GetComponent(const ComponentType& rComponentType) const;

	friend class Component;

private:
	AABB mBoundingVolume;
	Transform* mpTransform;
	Renderer* mpRenderer;
	std::vector<Component*> mComponents;
	bool mActive;
	
	GameObject();
	~GameObject();

	void AddComponent(Component* pComponent);
	void RemoveComponent(Component* pComponent);

	void DestroyAllComponents();

};

#endif