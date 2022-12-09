#ifndef FASTCG_COMPONENT_H
#define FASTCG_COMPONENT_H

#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>

#include <string>

#define FASTCG_DECLARE_COMPONENT(className, baseClassName)                        \
public:                                                                           \
	static const FastCG::ComponentType TYPE;                                      \
	inline const FastCG::ComponentType &GetType() const override                  \
	{                                                                             \
		return TYPE;                                                              \
	}                                                                             \
	template <typename... ArgsT>                                                  \
	static className *Instantiate(FastCG::GameObject *pGameObject, ArgsT... args) \
	{                                                                             \
		className *pComponent = new className(pGameObject, args...);              \
		pComponent->OnInstantiate();                                              \
		FastCG::Component::AddToGameObject(pGameObject, pComponent);              \
		return pComponent;                                                        \
	}                                                                             \
                                                                                  \
private:                                                                          \
	className(FastCG::GameObject *pGameObject) : baseClassName(pGameObject)       \
	{                                                                             \
	}                                                                             \
	virtual ~className() = default

#define FASTCG_DECLARE_ABSTRACT_COMPONENT(className, baseClassName)         \
public:                                                                     \
	static const FastCG::ComponentType TYPE;                                \
	inline const FastCG::ComponentType &GetType() const override            \
	{                                                                       \
		return TYPE;                                                        \
	}                                                                       \
                                                                            \
protected:                                                                  \
	className(FastCG::GameObject *pGameObject) : baseClassName(pGameObject) \
	{                                                                       \
	}                                                                       \
	virtual ~className() = default

#define FASTCG_IMPLEMENT_COMPONENT(className, baseClassName) \
	const FastCG::ComponentType className::TYPE(#className, &baseClassName::TYPE)

namespace FastCG
{
	class ComponentType
	{
	public:
		ComponentType(const std::string &rName, const ComponentType *pBaseType) : mName(rName),
																				  mpBaseType(pBaseType)
		{
		}

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline bool IsExactly(const ComponentType &rType) const
		{
			return &rType == this;
		}

		inline const ComponentType *GetBaseType() const
		{
			return mpBaseType;
		}

		inline bool IsDerived(const ComponentType &rType) const
		{
			const ComponentType *pSearch = this;
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
		const ComponentType *mpBaseType;
	};

	class Component
	{
	public:
		static const ComponentType TYPE;

		virtual ~Component() = default;

		inline virtual const ComponentType &GetType() const
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

		inline GameObject *GetGameObject()
		{
			return mpGameObject;
		}

		inline const GameObject *GetGameObject() const
		{
			return mpGameObject;
		}

		static void Destroy(Component *pComponent)
		{
			pComponent->OnDestroy();
			pComponent->RemoveFromParent();
			delete pComponent;
		}

	protected:
		static void AddToGameObject(GameObject *pGameObject, Component *pComponent)
		{
			pGameObject->AddComponent(pComponent);
		}

		Component(GameObject *pGameObject) : mpGameObject(pGameObject)
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
		GameObject *mpGameObject;
		bool mEnabled{true};
	};

}

#endif