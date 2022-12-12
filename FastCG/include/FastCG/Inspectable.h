#ifndef FASTCG_INSPECTABLE_H
#define FASTCG_INSPECTABLE_H

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>
#include <iterator>
#include <initializer_list>
#include <cstdint>
#include <algorithm>

namespace FastCG
{
    template <typename T, typename U>
    using InspectablePropertyGetter = U (T::*)() const;
    template <typename T, typename U>
    using InspectablePropertySetter = void (T::*)(U);
    template <typename T, typename U>
    using InspectablePropertyGetterCR = const U &(T::*)() const;
    template <typename T, typename U>
    using InspectablePropertySetterCR = void (T::*)(const U &);
    template <typename T, typename U>
    using InspectablePropertyGetterCP = const U *(T::*)() const;
    template <typename T, typename U>
    using InspectablePropertySetterCP = void (T::*)(const U *);

    enum class InspectablePropertyType : uint8_t
    {
        INSPECTABLE = 0,
        ENUM,
        BOOL,
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

    template <typename T, bool DerivedFromInspectable>
    struct _GetInspectablePropertyTypeFromType;

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum)                        \
    template <>                                                                         \
    struct _GetInspectablePropertyTypeFromType<type, false>                             \
    {                                                                                   \
        static const InspectablePropertyType value = InspectablePropertyType::typeEnum; \
    }

    template <typename T>
    struct _GetInspectablePropertyTypeFromType<T, true>
    {
        static const InspectablePropertyType value = InspectablePropertyType::INSPECTABLE;
    };

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

    class Inspectable;

    template <typename T>
    struct GetInspectablePropertyTypeFromType : _GetInspectablePropertyTypeFromType<T, std::is_base_of<Inspectable, T>::value>
    {
    };

    class IInspectableProperty
    {
    public:
        virtual InspectablePropertyType GetType() const = 0;
        virtual const std::string &GetName() const = 0;
        virtual void GetValue(void *value) const = 0;
        virtual void SetValue(const void *value) = 0;
        virtual bool IsReadOnly() const = 0;
    };

    class IInspectableEnumProperty : public IInspectableProperty
    {
    public:
        virtual size_t GetItemCount() const = 0;
        virtual const char *const *GetItems() const = 0;
        virtual size_t GetSelectedItem() const = 0;
        virtual void SetSelectedItem(size_t selectedItem) = 0;
    };

    class IInspectableDelimitedProperty : public IInspectableProperty
    {
    public:
        virtual void GetMin(void *value) const = 0;
        virtual void GetMax(void *value) const = 0;
    };

    template <typename T, typename U>
    class BaseInspectableProperty
    {
    protected:
        T *mpOwner;
        std::string mName;

        BaseInspectableProperty(T *pOwner, const std::string &rName)
            : mpOwner(pOwner),
              mName(rName)
        {
        }
    };

    template <typename T, typename U>
    class InspectableProperty : public BaseInspectableProperty<T, U>, public IInspectableDelimitedProperty
    {
    public:
        InspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter, const U &rMin, const U &rMax)
            : BaseInspectableProperty<T, U>(pOwner, rName),
              mMin(rMin),
              mMax(rMax),
              mGetter(rGetter),
              mSetter(rSetter)
        {
        }

        inline InspectablePropertyType GetType() const
        {
            return GetInspectablePropertyTypeFromType<U>::value;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<T, U>::mName;
        }

        inline U GetValue() const
        {
            return (BaseInspectableProperty<T, U>::mpOwner->*mGetter)();
        }

        inline void SetValue(U value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<T, U>::mpOwner->*mSetter)(value);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *value) const override
        {
            assert(value != nullptr);
            assert((intptr_t)value % alignof(U) == 0);
            *reinterpret_cast<U *>(value) = GetValue();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *value) override
        {
            assert(!IsReadOnly());
            assert(value != nullptr);
            assert((intptr_t)value % alignof(U) == 0);
            SetValue(*reinterpret_cast<const U *>(value));
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

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

    private:
        U mMin;
        U mMax;
        InspectablePropertyGetter<T, U> mGetter;
        InspectablePropertySetter<T, U> mSetter;
    };

    template <typename T, typename U>
    class InspectablePropertyCR : public BaseInspectableProperty<T, U>, public IInspectableDelimitedProperty
    {
    public:
        InspectablePropertyCR(T *pOwner, const std::string &rName, const InspectablePropertyGetterCR<T, U> &rGetter, const InspectablePropertySetterCR<T, U> &rSetter, const U &rMin, const U &rMax)
            : BaseInspectableProperty<T, U>(pOwner, rName),
              mMin(rMin),
              mMax(rMin),
              mGetter(rGetter),
              mSetter(rSetter)
        {
        }

        inline InspectablePropertyType GetType() const
        {
            return GetInspectablePropertyTypeFromType<U>::value;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<T, U>::mName;
        }

        inline const U &GetValueCR() const
        {
            return (BaseInspectableProperty<T, U>::mpOwner->*mGetter)();
        }

        inline void SetValueCR(const U &value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<T, U>::mpOwner->*mSetter)(value);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *value) const override
        {
            assert(value != nullptr);
            assert((intptr_t)value % alignof(U) == 0);
            *reinterpret_cast<U *>(value) = GetValueCR();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *value) override
        {
            assert(!IsReadOnly());
            assert(value != nullptr);
            assert((intptr_t)value % alignof(U) == 0);
            SetValueCR(*reinterpret_cast<const U *>(value));
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

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

    private:
        U mMin;
        U mMax;
        InspectablePropertyGetterCR<T, U> mGetter;
        InspectablePropertySetterCR<T, U> mSetter;
    };

    template <typename T, typename U>
    class InspectablePropertyCP : public BaseInspectableProperty<T, U>, public IInspectableProperty
    {
    public:
        InspectablePropertyCP(T *pOwner, const std::string &rName, const InspectablePropertyGetterCP<T, U> &rGetter, const InspectablePropertySetterCP<T, U> &rSetter)
            : BaseInspectableProperty<T, U>(pOwner, rName),
              mGetter(rGetter),
              mSetter(rSetter)
        {
        }

        inline InspectablePropertyType GetType() const
        {
            return GetInspectablePropertyTypeFromType<U>::value;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<T, U>::mName;
        }

        inline const U *GetValueCP() const
        {
            return (BaseInspectableProperty<T, U>::mpOwner->*mGetter)();
        }

        inline void SetValueCP(const U *value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<T, U>::mpOwner->*mSetter)(value);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *value) const override
        {
            assert(value != nullptr);
            assert((intptr_t)value % alignof(const U *) == 0);
            *reinterpret_cast<const U **>(value) = GetValueCP();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *value) override
        {
            assert(!IsReadOnly());
            assert(value != nullptr);
            assert((intptr_t)value % alignof(const U *) == 0);
            SetValueCP(*reinterpret_cast<const U **>(&value));
        }

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

    private:
        InspectablePropertyGetterCP<T, U> mGetter;
        InspectablePropertySetterCP<T, U> mSetter;
    };

    template <typename T>
    using InspectableEnumPropertyItem = std::pair<T, std::string>;

    template <typename T, typename U>
    class InspectableEnumProperty : public BaseInspectableProperty<T, U>, public IInspectableEnumProperty
    {
    public:
        InspectableEnumProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter, const std::initializer_list<InspectableEnumPropertyItem<U>> &rItems) : BaseInspectableProperty<T, U>(pOwner, rName),
                                                                                                                                                                                                                                            mGetter(rGetter),
                                                                                                                                                                                                                                            mSetter(rSetter)
        {
            std::transform(rItems.begin(), rItems.end(), std::back_inserter(mItemValues), [](const auto &rItem)
                           { return rItem.first; });
            std::transform(rItems.begin(), rItems.end(), std::back_inserter(mItemNames), [](const auto &rItem)
                           { return rItem.second; });
            std::transform(
                mItemNames.cbegin(), mItemNames.cend(), std::back_inserter(mItemNamePtrs), [](const auto &rItemName)
                { return rItemName.c_str(); });
        }

        inline InspectablePropertyType GetType() const override
        {
            return InspectablePropertyType::ENUM;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<T, U>::mName;
        }

        size_t GetItemCount() const override
        {
            return mItemNames.size();
        }

        const char *const *GetItems() const override
        {
            return &mItemNamePtrs[0];
        }

        inline U GetValue() const
        {
            return (BaseInspectableProperty<T, U>::mpOwner->*mGetter)();
        }

        inline void SetValue(U value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<T, U>::mpOwner->*mSetter)(value);
        }

        inline size_t GetSelectedItem() const override
        {
            auto value = GetValue();
            for (size_t idx = 0; idx < mItemValues.size(); idx++)
            {
                if (mItemValues[idx] == value)
                {
                    return idx;
                }
            }
            return size_t(~0);
        }

        inline void SetSelectedItem(size_t selectedItem) override
        {
            assert(selectedItem < mItemValues.size());
            SetValue(mItemValues[selectedItem]);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *value) const override
        {
            assert(value != nullptr);
            assert((intptr_t)value % alignof(U) == 0);
            *reinterpret_cast<U *>(value) = GetValue();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *value) override
        {
            assert(!IsReadOnly());
            assert(value != nullptr);
            assert((intptr_t)value % alignof(U) == 0);
            SetValue(*reinterpret_cast<const U *>(value));
        }

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

    private:
        InspectablePropertyGetter<T, U> mGetter;
        InspectablePropertySetter<T, U> mSetter;
        std::vector<U> mItemValues;
        std::vector<std::string> mItemNames;
        std::vector<const char *> mItemNamePtrs;
    };

    class Inspectable
    {
    public:
        virtual ~Inspectable() = default;

#ifdef _DEBUG
        inline size_t GetInspectablePropertyCount() const
        {
            return mInspectableProperties.size();
        }

        inline const IInspectableProperty *GetInspectableProperty(size_t i) const
        {
            return const_cast<Inspectable *>(this)->GetInspectableProperty(i);
        }

        inline IInspectableProperty *GetInspectableProperty(size_t i)
        {
            assert(i < mInspectableProperties.size());
            return mInspectableProperties[i].get();
        }
#endif

    protected:
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
        template <typename T, typename U>
        inline void RegisterInspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter = nullptr, const U &rMin = std::numeric_limits<U>::min(), const U &rMax = std::numeric_limits<U>::max())
        {
#ifdef _DEBUG
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(new InspectableProperty<T, U>(pOwner, rName, rGetter, rSetter, rMin, rMax));
#endif
        }

        template <typename T, typename U>
        inline void RegisterInspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetterCR<T, U> &rGetterCR, const InspectablePropertySetterCR<T, U> &rSetterCR = nullptr, const U &rMin = std::numeric_limits<U>::min(), const U &rMax = std::numeric_limits<U>::max())
        {
#ifdef _DEBUG
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(new InspectablePropertyCR<T, U>(pOwner, rName, rGetterCR, rSetterCR, rMin, rMax));
#endif
        }

        template <typename T, typename U>
        inline void RegisterInspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetterCP<T, U> &rGetterCP, const InspectablePropertySetterCP<T, U> &rSetterCP = nullptr)
        {
#ifdef _DEBUG
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(new InspectablePropertyCP<T, U>(pOwner, rName, rGetterCP, rSetterCP));
#endif
        }

        template <typename T, typename U>
        inline void RegisterInspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const InspectablePropertySetter<T, U> &rSetter, const std::initializer_list<InspectableEnumPropertyItem<U>> &rItems)
        {
#ifdef _DEBUG
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(new InspectableEnumProperty<T, U>(pOwner, rName, rGetter, rSetter, rItems));
#endif
        }

        template <typename T, typename U>
        inline void RegisterInspectableProperty(T *pOwner, const std::string &rName, const InspectablePropertyGetter<T, U> &rGetter, const std::initializer_list<InspectableEnumPropertyItem<U>> &rItems)
        {
#ifdef _DEBUG
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(new InspectableEnumProperty<T, U>(pOwner, rName, rGetter, nullptr, rItems));
#endif
        }

        template <typename T, typename U>
        inline void UnregisterInspectableProperty(const std::string &rName)
        {
#ifdef _DEBUG
            auto it = GetInspectablePropertyIterator(rName);
            assert(it != mInspectableProperties.cend());
            mInspectableProperties.erase(it);
#endif
        }

    private:
#ifdef _DEBUG
        std::vector<std::unique_ptr<IInspectableProperty>> mInspectableProperties;

        auto GetInspectablePropertyIterator(const std::string &rName) const
        {
            return std::find_if(mInspectableProperties.cbegin(), mInspectableProperties.cend(), [&rName](const auto &pInspectableProperty)
                                { return pInspectableProperty->GetName() == rName; });
        }
#endif
    };
}

#endif