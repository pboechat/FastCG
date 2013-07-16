#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <AABB.h>

#include <string>
#include <vector>

class ComponentType;
class Component;
class Transform;
class Renderer;

class GameObject
{
public:
	static GameObject* Instantiate();
	static GameObject* Instantiate(const std::string& rName);
	static void Destroy(GameObject* pGameObject);

	inline const std::string& GetName() const
	{
		return mName;
	}

	inline void SetName(const std::string& rName)
	{
		mName = rName;
	}

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

	void SetActive(bool active);
	Component* GetComponent(const ComponentType& rComponentType) const;
	friend class Component;

private:
	std::string mName;
	AABB mBoundingVolume;
	Transform* mpTransform;
	Renderer* mpRenderer;
	std::vector<Component*> mComponents;
	bool mActive;
	
	GameObject();
	GameObject(const std::string& rName);
	~GameObject();
	void AddComponent(Component* pComponent);
	void RemoveComponent(Component* pComponent);
	void DestroyAllComponents();

};

#endif