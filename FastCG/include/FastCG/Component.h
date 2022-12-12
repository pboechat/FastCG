#ifndef FASTCG_COMPONENT_H
#define FASTCG_COMPONENT_H

#include <FastCG/Inspectable.h>
#include <FastCG/GameObject.h>
#include <FastCG/Exception.h>

#include <string>
#include <algorithm>

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

	template <typename T, typename U>
	using InspectablePropertyGetter = U (T::*)() const;
	template <typename T, typename U>
	using InspectablePropertySetter = void (T::*)(U);
	template <typename T, typename U>
	using InspectablePropertyGetterCR = const U &(T::*)() const;
	template <typename T, typename U>
	using InspectablePropertySetterCR = void (T::*)(const U &);

#ifdef _DEBUG
	enum class InspectablePropertyType : uint8_t
	{
		BOOL = 0,
		STRING,
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
	struct GetInspectablePropertyTypeFromType;

	template <InspectablePropertyType TypeEnum>
	struct GetTypeFromInspectablePropertyType;

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum)                          \
	template <>                                                                           \
	struct GetInspectablePropertyTypeFromType<type>                                       \
	{                                                                                     \
		static const InspectablePropertyType value = InspectablePropertyType::##typeEnum; \
	};                                                                                    \
	template <>                                                                           \
	struct GetTypeFromInspectablePropertyType<InspectablePropertyType::##typeEnum>        \
	{                                                                                     \
		using value = type;                                                               \
	}

	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(bool, BOOL);
	FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(std::string, STRING);
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
		virtual void GetMin(void *value) const = 0;
		virtual void GetMax(void *value) const = 0;
		virtual void GetValue(void *value) const = 0;
		virtual void SetValue(const void *value) = 0;
		virtual bool IsReadOnly() const = 0;
	};

	template <typename T, typename U>
	class InspectableProperty : public IInspectableProperty
	{
	public:
		static const auto TYPE = GetInspectablePropertyTypeFromType<U>::value;

		InspectableProperty(T *pOwner, const std::string &rName, U min, U max, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter)
			: mpOwner(pOwner),
			  mName(rName),
			  mMin(min),
			  mMax(max),
			  mGetter(rGetter),
			  mSetter(rSetter),
			  mIsCR(false)
		{
		}

		InspectableProperty(T *pOwner, const std::string &rName, const U &min, const U &max, const InspectablePropertyGetterCR<T, U> &rGetterCR, const InspectablePropertySetterCR<T, U> &rSetterCR)
			: mpOwner(pOwner),
			  mName(rName),
			  mMin(min),
			  mMax(max),
			  mGetterCR(rGetterCR),
			  mSetterCR(rSetterCR),
			  mIsCR(true)
		{
		}

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
			assert(!mIsCR);
			return (mpOwner->*mGetter)();
		}

		inline void SetValue(U value)
		{
			assert(!mIsCR);
			assert(!IsReadOnly());
			(mpOwner->*mSetter)(value);
		}

		inline const U &GetValueCR() const
		{
			assert(mIsCR);
			return (mpOwner->*mGetterCR)();
		}

		inline void SetValueCR(const U &value)
		{
			assert(mIsCR);
			assert(!IsReadOnly());
			(mpOwner->*mSetterCR)(value);
		}

		// This method can cause misaligned accesses!
		inline void GetMin(void *value) const override
		{
			assert(value != nullptr);
			assert((intptr_t)value % alignof(U) == 0);
			*reinterpret_cast<U *>(value) = mMin;
		}

		// This method can cause misaligned accesses!
		inline void GetMax(void *value) const override
		{
			assert(value != nullptr);
			assert((intptr_t)value % alignof(U) == 0);
			*reinterpret_cast<U *>(value) = mMax;
		}

		// This method can cause misaligned accesses!
		inline void GetValue(void *value) const override
		{
			assert(value != nullptr);
			assert((intptr_t)value % alignof(U) == 0);
			if (mIsCR)
			{
				*reinterpret_cast<U *>(value) = GetValueCR();
			}
			else
			{
				*reinterpret_cast<U *>(value) = GetValue();
			}
		}

		// This method can cause misaligned accesses!
		inline void SetValue(const void *value) override
		{
			assert(!IsReadOnly());
			assert(value != nullptr);
			assert((intptr_t)value % alignof(U) == 0);
			if (mIsCR)
			{
				SetValueCR(*reinterpret_cast<const U *>(value));
			}
			else
			{
				SetValue(*reinterpret_cast<const U *>(value));
			}
		}

		inline bool IsReadOnly() const override
		{
			return (mIsCR && mSetterCR == nullptr) || mSetter == nullptr;
		}

	private:
		T *mpOwner;
		std::string mName;
		U mMin;
		U mMax;
		union
		{
			InspectablePropertyGetter<T, U> mGetter;
			InspectablePropertyGetterCR<T, U> mGetterCR;
		};
		union
		{
			InspectablePropertySetter<T, U> mSetter;
			InspectablePropertySetterCR<T, U> mSetterCR;
		};
		bool mIsCR : 1;
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
		virtual void OnRegisterInspectableProperties()
		{
		}

	private:
		GameObject *mpGameObject;
		bool mEnabled{true};
	};

}

#endif