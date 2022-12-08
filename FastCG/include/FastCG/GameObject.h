#ifndef FASTCG_GAME_OBJECT_H
#define FASTCG_GAME_OBJECT_H

#include <FastCG/AABB.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>
#include <memory>

namespace FastCG
{
	class ComponentType;
	class Component;
	class Transform;
	class Renderable;

	class GameObject
	{
	public:
		static GameObject *Instantiate(const std::string &rName = {}, const glm::vec3 &rScale = glm::vec3{1, 1, 1}, const glm::quat &rRotation = {}, const glm::vec3 &rPosition = glm::vec3{0, 0, 0});
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
			return mpTransform.get();
		}

		inline const Transform *GetTransform() const
		{
			return mpTransform.get();
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
			return static_cast<ComponentT *>(GetComponent(ComponentT::TYPE));
		}
		AABB GetBounds() const;
		friend class Component;

	private:
		using TransformUniquePtr = std::unique_ptr<Transform, void (*)(void *)>;

		std::string mName;
		TransformUniquePtr mpTransform;
		Renderable *mpRenderable{nullptr};
		std::vector<Component *> mComponents;
		bool mActive{true};

		GameObject();
		GameObject(const std::string &rName, const glm::vec3 &rScale, const glm::quat &rRotation, const glm::vec3 &rPosition);
		~GameObject();

		void AddComponent(Component *pComponent);
		void RemoveComponent(Component *pComponent);
		void DestroyAllComponents();
	};

}

#endif