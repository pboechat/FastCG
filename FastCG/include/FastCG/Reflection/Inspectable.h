#ifndef FASTCG_INSPECTABLE_H
#define FASTCG_INSPECTABLE_H

#include <FastCG/Core/Enums.h>
#include <FastCG/Rendering/Material.h>
#include <FastCG/Rendering/Mesh.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <string.h>
#include <string>
#include <type_traits>
#include <vector>

namespace FastCG
{
    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    using InspectablePropertyGetter = InspectablePropertyTypeT (InspectablePropertyOwnerT::*)() const;
    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    using InspectablePropertySetter = void (InspectablePropertyOwnerT::*)(InspectablePropertyTypeT);
    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    using InspectablePropertyGetterCR = const InspectablePropertyTypeT &(InspectablePropertyOwnerT::*)() const;
    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    using InspectablePropertySetterCR = void (InspectablePropertyOwnerT::*)(const InspectablePropertyTypeT &);
    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    using InspectablePropertyGetterCRP = const InspectablePropertyTypeT *(InspectablePropertyOwnerT::*)() const;
    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    using InspectablePropertySetterCRP = void (InspectablePropertyOwnerT::*)(const InspectablePropertyTypeT *);

    FASTCG_DECLARE_SCOPED_ENUM(InspectablePropertyType, uint8_t, BOOL, ENUM, INT32, UINT32, INT64, UINT64, FLOAT,
                               DOUBLE, VEC2, VEC3, VEC4, STRING, INSPECTABLE, MATERIAL, MESH, TEXTURE);

    template <typename InspectablePropertyTypeT, bool IsDerivedFromInspectable>
    struct _GetInspectablePropertyTypeFromType;

    FASTCG_DECLARE_SCOPED_ENUM(InspectablePropertyQualifier, uint8_t, CONSTANT, REFERENCE, RAW_POINTER);

    FASTCG_DECLARE_SCOPED_ENUM(InspectablePropertyQualifierRequirement, uint8_t, NONE, MUST, MUSTNT);

    template <typename InspectablePropertyTypeT, InspectablePropertyQualifier QualifierT,
              bool IsDerivedFromInspectable = false>
    struct GetInspectablePropertyQualifierRequirement;

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum, constQualifier, refQualifier, ptrQualifier)           \
    template <>                                                                                                        \
    struct _GetInspectablePropertyTypeFromType<type, false>                                                            \
    {                                                                                                                  \
        static constexpr auto value = InspectablePropertyType::typeEnum;                                               \
    };                                                                                                                 \
    template <>                                                                                                        \
    struct GetInspectablePropertyQualifierRequirement<type, InspectablePropertyQualifier::CONSTANT, false>             \
    {                                                                                                                  \
        static constexpr auto value = InspectablePropertyQualifierRequirement::constQualifier;                         \
    };                                                                                                                 \
    template <>                                                                                                        \
    struct GetInspectablePropertyQualifierRequirement<type, InspectablePropertyQualifier::REFERENCE, false>            \
    {                                                                                                                  \
        static constexpr auto value = InspectablePropertyQualifierRequirement::refQualifier;                           \
    };                                                                                                                 \
    template <>                                                                                                        \
    struct GetInspectablePropertyQualifierRequirement<type, InspectablePropertyQualifier::RAW_POINTER, false>          \
    {                                                                                                                  \
        static constexpr auto value = InspectablePropertyQualifierRequirement::ptrQualifier;                           \
    }

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(type, typeEnum)                                            \
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum, NONE, MUSTNT, MUSTNT)

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(type, typeEnum)                               \
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum, MUST, MUST, MUSTNT)

#define FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_RAW_POINTER_ONLY_TYPE(type, typeEnum)                             \
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_TYPE(type, typeEnum, MUST, MUSTNT, MUST)

    template <typename InspectablePropertyTypeT>
    struct _GetInspectablePropertyTypeFromType<InspectablePropertyTypeT, true>
    {
        static constexpr InspectablePropertyType value = InspectablePropertyType::INSPECTABLE;
    };

    template <typename InspectablePropertyTypeT>
    struct GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT, InspectablePropertyQualifier::CONSTANT,
                                                      true>
    {
        static constexpr auto value = InspectablePropertyQualifierRequirement::MUST;
    };

    template <typename InspectablePropertyTypeT>
    struct GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT, InspectablePropertyQualifier::REFERENCE,
                                                      true>
    {
        static constexpr auto value = InspectablePropertyQualifierRequirement::MUSTNT;
    };

    template <typename InspectablePropertyTypeT>
    struct GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                      InspectablePropertyQualifier::RAW_POINTER, true>
    {
        static constexpr auto value = InspectablePropertyQualifierRequirement::MUST;
    };

    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(bool, BOOL);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(int32_t, INT32);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(uint32_t, UINT32);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(int64_t, INT64);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(uint64_t, UINT64);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(float, FLOAT);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_VALUE_ONLY_TYPE(double, DOUBLE);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(glm::vec2, VEC2);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(glm::vec3, VEC3);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(glm::vec4, VEC4);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(std::string, STRING);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(std::shared_ptr<Material>, MATERIAL);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_REFERENCE_ONLY_TYPE(std::shared_ptr<Mesh>, MESH);
    FASTCG_DECLARE_INSPECTABLE_PROPERTY_CONSTANT_RAW_POINTER_ONLY_TYPE(Texture, TEXTURE);

    class Inspectable;

    template <typename InspectablePropertyTypeT>
    struct GetInspectablePropertyTypeFromType
        : _GetInspectablePropertyTypeFromType<InspectablePropertyTypeT,
                                              std::is_base_of<Inspectable, InspectablePropertyTypeT>::value>
    {
    };

    class IInspectableProperty
    {
    public:
        virtual ~IInspectableProperty() = default;

        virtual InspectablePropertyType GetType() const = 0;
        virtual const std::string &GetName() const = 0;
        virtual void GetValue(void *pValue) const = 0;
        virtual void SetValue(const void *pValue) = 0;
        virtual bool IsReadOnly() const = 0;
        virtual bool IsConst() const = 0;
        virtual bool IsRef() const = 0;
        virtual bool IsRawPtr() const = 0;
    };

    class IInspectableEnumProperty : public IInspectableProperty
    {
    public:
        virtual size_t GetItemCount() const = 0;
        virtual const char *const *GetItems() const = 0;
        virtual size_t GetSelectedItem() const = 0;
        virtual const char *GetSelectedItemName() const = 0;
        virtual void SetSelectedItem(size_t selectedItem) = 0;
        virtual void SetSelectedItem(const char *) = 0;
    };

    class IInspectableDelimitedProperty : public IInspectableProperty
    {
    public:
        virtual void GetMin(void *pValue) const = 0;
        virtual void GetMax(void *pValue) const = 0;
    };

    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    class BaseInspectableProperty
    {
    protected:
        InspectablePropertyOwnerT *mpOwner;
        std::string mName;

        BaseInspectableProperty(InspectablePropertyOwnerT *pOwner, const std::string &rName)
            : mpOwner(pOwner), mName(rName)
        {
        }
    };

    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    class InspectableProperty : public BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>,
                                public IInspectableDelimitedProperty
    {
    public:
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::CONSTANT>::value !=
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be constant");
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::REFERENCE>::value !=
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be a reference");
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::RAW_POINTER>::value !=
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be a raw pointer");

        InspectableProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const InspectablePropertySetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetter,
            const InspectablePropertyTypeT &rMin, const InspectablePropertyTypeT &rMax)
            : BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName), mMin(rMin),
              mMax(rMax), mGetter(rGetter), mSetter(rSetter)
        {
        }

        inline InspectablePropertyType GetType() const override
        {
            return GetInspectablePropertyTypeFromType<InspectablePropertyTypeT>::value;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mName;
        }

        inline InspectablePropertyTypeT GetValue() const
        {
            return (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mGetter)();
        }

        inline void SetValue(InspectablePropertyTypeT value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mSetter)(value);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = GetValue();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *pValue) override
        {
            assert(!IsReadOnly());
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            SetValue(*reinterpret_cast<const InspectablePropertyTypeT *>(pValue));
        }

        // This method can cause misaligned accesses!
        inline void GetMin(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = mMin;
        }

        // This method can cause misaligned accesses!
        inline void GetMax(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = mMax;
        }

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

        inline bool IsConst() const override
        {
            return false;
        }

        inline bool IsRef() const override
        {
            return false;
        }

        inline bool IsRawPtr() const override
        {
            return false;
        }

    private:
        InspectablePropertyTypeT mMin;
        InspectablePropertyTypeT mMax;
        InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> mGetter;
        InspectablePropertySetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> mSetter;
    };

    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    class InspectablePropertyCR : public BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>,
                                  public IInspectableDelimitedProperty
    {
    public:
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::CONSTANT>::value !=
                          InspectablePropertyQualifierRequirement::MUSTNT,
                      "Inspectable property type mustn't be constant");
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::REFERENCE>::value !=
                          InspectablePropertyQualifierRequirement::MUSTNT,
                      "Inspectable property type mustn't be a reference");
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::RAW_POINTER>::value !=
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be a raw pointer");

        InspectablePropertyCR(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetterCR<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const InspectablePropertySetterCR<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetter,
            const InspectablePropertyTypeT &rMin, const InspectablePropertyTypeT &rMax)
            : BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName), mMin(rMin),
              mMax(rMax), mGetter(rGetter), mSetter(rSetter)
        {
        }

        inline InspectablePropertyType GetType() const override
        {
            return GetInspectablePropertyTypeFromType<InspectablePropertyTypeT>::value;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mName;
        }

        inline const InspectablePropertyTypeT &GetValueCR() const
        {
            return (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mGetter)();
        }

        inline void SetValueCR(const InspectablePropertyTypeT &value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mSetter)(value);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = GetValueCR();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *pValue) override
        {
            assert(!IsReadOnly());
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            SetValueCR(*reinterpret_cast<const InspectablePropertyTypeT *>(pValue));
        }

        // This method can cause misaligned accesses!
        inline void GetMin(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = mMin;
        }

        // This method can cause misaligned accesses!
        inline void GetMax(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = mMax;
        }

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

        inline bool IsConst() const override
        {
            return true;
        }

        inline bool IsRef() const override
        {
            return true;
        }

        inline bool IsRawPtr() const override
        {
            return false;
        }

    private:
        InspectablePropertyTypeT mMin;
        InspectablePropertyTypeT mMax;
        InspectablePropertyGetterCR<InspectablePropertyOwnerT, InspectablePropertyTypeT> mGetter;
        InspectablePropertySetterCR<InspectablePropertyOwnerT, InspectablePropertyTypeT> mSetter;
    };

    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    class InspectablePropertyCRP : public BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>,
                                   public IInspectableProperty
    {
    public:
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::CONSTANT>::value ==
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be constant");
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::REFERENCE>::value !=
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be a reference");
        static_assert(GetInspectablePropertyQualifierRequirement<InspectablePropertyTypeT,
                                                                 InspectablePropertyQualifier::RAW_POINTER>::value ==
                          InspectablePropertyQualifierRequirement::MUST,
                      "Inspectable property type must be a raw pointer");

        InspectablePropertyCRP(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetterCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const InspectablePropertySetterCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetter)
            : BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName),
              mGetter(rGetter), mSetter(rSetter)
        {
        }

        inline InspectablePropertyType GetType() const
        {
            return GetInspectablePropertyTypeFromType<InspectablePropertyTypeT>::value;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mName;
        }

        inline const InspectablePropertyTypeT *GetValueCRP() const
        {
            return (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mGetter)();
        }

        inline void SetValueCRP(const InspectablePropertyTypeT *pValue)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mSetter)(pValue);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(const InspectablePropertyTypeT *) == 0);
            *reinterpret_cast<const InspectablePropertyTypeT **>(pValue) = GetValueCRP();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *pValue) override
        {
            assert(!IsReadOnly());
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(const InspectablePropertyTypeT *) == 0);
            SetValueCRP(*reinterpret_cast<const InspectablePropertyTypeT *const *>(pValue));
        }

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

        inline bool IsConst() const override
        {
            return true;
        }

        inline bool IsRef() const override
        {
            return false;
        }

        inline bool IsRawPtr() const override
        {
            return true;
        }

    private:
        InspectablePropertyGetterCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT> mGetter;
        InspectablePropertySetterCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT> mSetter;
    };

    template <typename InspectablePropertyTypeT>
    using InspectableEnumPropertyItem = std::pair<InspectablePropertyTypeT, std::string>;

    template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
    class InspectableEnumProperty : public BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>,
                                    public IInspectableEnumProperty
    {
    public:
        static_assert(std::is_enum<InspectablePropertyTypeT>::value, "Inspectable property type must be an enum");

        InspectableEnumProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const InspectablePropertySetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetter,
            const std::initializer_list<InspectableEnumPropertyItem<InspectablePropertyTypeT>> &rItems)
            : BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName),
              mGetter(rGetter), mSetter(rSetter)
        {
            std::transform(rItems.begin(), rItems.end(), std::back_inserter(mItemValues),
                           [](const auto &rItem) { return rItem.first; });
            std::transform(rItems.begin(), rItems.end(), std::back_inserter(mItemNames),
                           [](const auto &rItem) { return rItem.second; });
            std::transform(mItemNames.cbegin(), mItemNames.cend(), std::back_inserter(mItemNamePtrs),
                           [](const auto &rItemName) { return rItemName.c_str(); });
        }

        inline InspectablePropertyType GetType() const override
        {
            return InspectablePropertyType::ENUM;
        }

        inline const std::string &GetName() const override
        {
            return BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mName;
        }

        size_t GetItemCount() const override
        {
            return mItemNames.size();
        }

        const char *const *GetItems() const override
        {
            return &mItemNamePtrs[0];
        }

        inline InspectablePropertyTypeT GetValue() const
        {
            return (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mGetter)();
        }

        inline void SetValue(InspectablePropertyTypeT value)
        {
            assert(!IsReadOnly());
            (BaseInspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>::mpOwner->*mSetter)(value);
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

        inline const char *GetSelectedItemName() const override
        {
            auto value = GetValue();
            for (size_t idx = 0; idx < mItemValues.size(); idx++)
            {
                if (mItemValues[idx] == value)
                {
                    return mItemNamePtrs[idx];
                }
            }
            return nullptr;
        }

        inline void SetSelectedItem(size_t selectedItem) override
        {
            assert(selectedItem < mItemValues.size());
            SetValue(mItemValues[selectedItem]);
        }

        inline void SetSelectedItem(const char *pSelectedItemName) override
        {
            auto it =
                std::find_if(mItemNamePtrs.cbegin(), mItemNamePtrs.cend(), [pSelectedItemName](const auto *pItemName) {
                    return strcmp(pSelectedItemName, pItemName) == 0;
                });
            assert(it != mItemNamePtrs.cend());
            auto idx = (size_t)std::distance(mItemNamePtrs.cbegin(), it);
            SetSelectedItem(idx);
        }

        // This method can cause misaligned accesses!
        inline void GetValue(void *pValue) const override
        {
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            *reinterpret_cast<InspectablePropertyTypeT *>(pValue) = GetValue();
        }

        // This method can cause misaligned accesses!
        inline void SetValue(const void *pValue) override
        {
            assert(!IsReadOnly());
            assert(pValue != nullptr);
            assert((intptr_t)pValue % alignof(InspectablePropertyTypeT) == 0);
            SetValue(*reinterpret_cast<const InspectablePropertyTypeT *>(pValue));
        }

        inline bool IsReadOnly() const override
        {
            return mSetter == nullptr;
        }

        inline bool IsConst() const override
        {
            return false;
        }

        inline bool IsRef() const override
        {
            return false;
        }

        inline bool IsRawPtr() const override
        {
            return false;
        }

    private:
        InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> mGetter;
        InspectablePropertySetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> mSetter;
        std::vector<InspectablePropertyTypeT> mItemValues;
        std::vector<std::string> mItemNames;
        std::vector<const char *> mItemNamePtrs;
    };

    class Inspectable
    {
    public:
        virtual ~Inspectable() = default;

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

        inline IInspectableProperty *GetInspectableProperty(const std::string &rName)
        {
            auto it = GetInspectablePropertyIterator(rName);
            if (it == mInspectableProperties.end())
            {
                return nullptr;
            }
            return it->get();
        }

    protected:
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
        template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
        inline void RegisterInspectableProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const InspectablePropertySetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetter = nullptr,
            const InspectablePropertyTypeT &rMin = std::numeric_limits<InspectablePropertyTypeT>::min(),
            const InspectablePropertyTypeT &rMax = std::numeric_limits<InspectablePropertyTypeT>::max())
        {
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            FASTCG_UNUSED(it);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(
                new InspectableProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName, rGetter,
                                                                                             rSetter, rMin, rMax));
        }

        template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
        inline void RegisterInspectableProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetterCR<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetterCR,
            const InspectablePropertySetterCR<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetterCR = nullptr,
            const InspectablePropertyTypeT &rMin = std::numeric_limits<InspectablePropertyTypeT>::min(),
            const InspectablePropertyTypeT &rMax = std::numeric_limits<InspectablePropertyTypeT>::max())
        {
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            FASTCG_UNUSED(it);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(
                new InspectablePropertyCR<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName, rGetterCR,
                                                                                               rSetterCR, rMin, rMax));
        }

        template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
        inline void RegisterInspectableProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetterCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetterCRP,
            const InspectablePropertySetterCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetterCRP =
                nullptr)
        {
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            FASTCG_UNUSED(it);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(
                new InspectablePropertyCRP<InspectablePropertyOwnerT, InspectablePropertyTypeT>(
                    pOwner, rName, rGetterCRP, rSetterCRP));
        }

        template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
        inline void RegisterInspectableProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const InspectablePropertySetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rSetter,
            const std::initializer_list<InspectableEnumPropertyItem<InspectablePropertyTypeT>> &rItems)
        {
            assert(pOwner != nullptr);
            auto it = GetInspectablePropertyIterator(rName);
            FASTCG_UNUSED(it);
            assert(it == mInspectableProperties.cend());
            mInspectableProperties.emplace_back(
                new InspectableEnumProperty<InspectablePropertyOwnerT, InspectablePropertyTypeT>(pOwner, rName, rGetter,
                                                                                                 rSetter, rItems));
        }

        template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
        inline void RegisterInspectableProperty(
            InspectablePropertyOwnerT *pOwner, const std::string &rName,
            const InspectablePropertyGetter<InspectablePropertyOwnerT, InspectablePropertyTypeT> &rGetter,
            const std::initializer_list<InspectableEnumPropertyItem<InspectablePropertyTypeT>> &rItems)
        {
            RegisterInspectableProperty(pOwner, rName, rGetter, nullptr, rItems);
        }

        template <typename InspectablePropertyOwnerT, typename InspectablePropertyTypeT>
        inline void UnregisterInspectableProperty(const std::string &rName)
        {
            auto it = GetInspectablePropertyIterator(rName);
            assert(it != mInspectableProperties.cend());
            mInspectableProperties.erase(it);
        }

    private:
        std::vector<std::unique_ptr<IInspectableProperty>> mInspectableProperties;

        auto GetInspectablePropertyIterator(const std::string &rName) -> decltype(mInspectableProperties.begin())
        {
            return std::find_if(
                mInspectableProperties.begin(), mInspectableProperties.end(),
                [&rName](const auto &pInspectableProperty) { return pInspectableProperty->GetName() == rName; });
        }
    };
}

#endif