#ifndef FASTCG_COMPONENT_H
#define FASTCG_COMPONENT_H

#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>

#include <string>

#ifdef _DEBUG
#define FASTCG_REGISTER_INSPECTABLE_PROPERTIES(component) \
	component->OnRegisterInspectableProperties()
#else
#define FASTCG_REGISTER_INSPECTABLE_PROPERTIES(component)
#endif

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
		FASTCG_REGISTER_INSPECTABLE_PROPERTIES(pComponent);                       \
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

#ifdef _DEBUG
	enum class InspectablePropertyType : uint8_t
	{
		BYTE_STRING = 0,
		CHAR_STRING,
		CHAR,
		UCHAR,
		INT32,
		UINT32,
		INT64,
		UINT64,
		FLOAT,
		DOUBLE,
		VEC2,
		VEC3,
		VEC4,

	};

	template <typename T>
	struct GetInspectablePropertyType;

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum)                          \
	template <>                                                                           \
	struct GetInspectablePropertyType<type>                                               \
	{                                                                                     \
		static const InspectablePropertyType value = InspectablePropertyType::##typeEnum; \
	}

	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(uint8_t *, BYTE_STRING);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(char *, CHAR_STRING);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(char, CHAR);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(unsigned char, UCHAR);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(int32_t, INT32);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(uint32_t, UINT32);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(int64_t, INT64);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(uint64_t, UINT64);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(float, FLOAT);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(double, DOUBLE);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(glm::vec2, VEC2);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(glm::vec3, VEC3);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(glm::vec4, VEC4);

	class IInspectableProperty
	{
	public:
		virtual InspectablePropertyType GetType() const = 0;
		virtual const std::string &GetName() const = 0;
		virtual void GetValue(void *value) const = 0;
		virtual void SetValue(const void *value) = 0;
	};

	template <typename T, typename U>
	using InspectablePropertyGetter = U (T::*)() const;
	template <typename T, typename U>
	using InspectablePropertySetter = void (T::*)(U);

	template <typename T, typename U>
	class InspectableProperty : public IInspectableProperty
	{
	public:
		static const auto TYPE = GetInspectablePropertyType<U>::value;

		InspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter) : mpOwner(pOwner), mName(rName), mGetter(rGetter), mSetter(rSetter) {}

		inline InspectablePropertyType GetType() const override
		{
			return TYPE;
		}

		inline const std::string &GetName() const override
		{
			return mName;
		}

		inline U GetValue() const
		{
			return (mpOwner->*mGetter)();
		}

		inline void SetValue(U value)
		{
			(mpOwner->*mSetter)(value);
		}

		inline void GetValue(void *value) const override
		{
			auto actualValue = GetValue();
			memcpy(value, (void*)&actualValue, sizeof(U));
		}
		virtual void SetValue(const void *value) override
		{
			SetValue(*reinterpret_cast<const U *>(value));
		}

	private:
		T *mpOwner;
		std::string mName;
		InspectablePropertyGetter<T, U> mGetter;
		InspectablePropertySetter<T, U> mSetter;
	};
#endif

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

#ifdef _DEBUG
		inline size_t GetInspectablePropertyCount() const
		{
			return mInspectableProperties.size();
		}

		inline const IInspectableProperty *GetInspectableProperty(size_t i) const
		{
			return const_cast<Component *>(this)->GetInspectableProperty(i);
		}

		inline IInspectableProperty *GetInspectableProperty(size_t i)
		{
			assert(i < mInspectableProperties.size());
			return mInspectableProperties[i].get();
		}
#endif

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

	protected:
#ifdef _DEBUG
		template <typename T, typename U>
		inline void RegisterInspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter)
		{
			auto it = GetInspectablePropertyIterator(rName);
			assert(it == mInspectableProperties.cend());
			mInspectableProperties.emplace_back(new InspectableProperty<T, U>(pOwner, rName, rGetter, rSetter));
		}
		template <typename T, typename U>
		inline void UnregisterInspectableProperty(const std::string &rName)
		{
			auto it = GetInspectablePropertyIterator(rName);
			assert(it != mInspectableProperties.cend());
			mInspectableProperties.erase(it);
		}
		virtual void OnRegisterInspectableProperties()
		{
		}
#endif

	private:
		GameObject *mpGameObject;
		bool mEnabled{true};
#ifdef _DEBUG
		std::vector<std::unique_ptr<IInspectableProperty>> mInspectableProperties;
#endif

#ifdef _DEBUG
		auto GetInspectablePropertyIterator(const std::string &rName) const
		{
			return std::find_if(mInspectableProperties.cbegin(), mInspectableProperties.cend(), [&rName](const auto &pInspectableProperty)
								{ return pInspectableProperty->GetName() == rName; });
		}
#endif
	};

}

#endif