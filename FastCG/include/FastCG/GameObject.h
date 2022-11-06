#ifndef FASTCG_GAME_OBJECT_H
#define FASTCG_GAME_OBJECT_H

#include <FastCG/AABB.h>

#include <string>
#include <vector>

namespace FastCG
{
	class ComponentType;
	class Component;
	class Transform;
	class Renderable;

	class GameObject
	{
	public:
		static GameObject *Instantiate();
		static GameObject *Instantiate(const std::string &rName);
		static void Destroy(GameObject *pGameObject);

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline void SetName(const std::string &rName)
		{
			mName = rName;
		}

		inline Transform *GetTransform()
		{
			return mpTransform;
		}

		inline const Transform *GetTransform() const
		{
			return mpTransform;
		}

		inline Renderable *GetRenderable()
		{
			return mpRenderable;
		}

		inline const Renderable *GetRenderable() const
		{
			return mpRenderable;
		}

		inline bool IsActive() const
		{
			return mActive;
		}

		void SetActive(bool active);
		Component *GetComponent(const ComponentType &rComponentType) const;
		template <class ComponentT>
		ComponentT *GetComponent() const
		{
			return static_cast<ComponentT*>(GetComponent(ComponentT::TYPE));
		}
		AABB GetBounds() const;
		friend class Component;

	private:
		std::string mName;
		Transform *mpTransform;
		Renderable *mpRenderable{nullptr};
		std::vector<Component *> mComponents;
		bool mActive{true};

		GameObject();
		GameObject(const std::string &rName);
		~GameObject();

		void AddComponent(Component *pComponent);
		void RemoveComponent(Component *pComponent);
		void DestroyAllComponents();
	};

}

#endif