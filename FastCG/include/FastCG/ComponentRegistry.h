#ifndef FASTCG_COMPONENT_REGISTRY_H
#define FASTCG_COMPONENT_REGISTRY_H

#include <FastCG/Component.h>

#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    using ComponentInstantiator = Component *(*)(GameObject *);

    class ComponentRegister
    {
    public:
        ComponentRegister(const ComponentType &rComponentType, ComponentInstantiator componentInstantiator)
            : mrComponentType(rComponentType),
              mComponentInstantiator(componentInstantiator)
        {
            assert(mComponentInstantiator != nullptr);
        }

        inline bool RefersTo(const ComponentType &rComponentType) const
        {
            return &mrComponentType == &rComponentType;
        }

        template <typename ComponentT>
        inline bool RefersTo() const
        {
            return RefersTo(ComponentT::TYPE);
        }

        inline const ComponentType &GetType() const
        {
            return mrComponentType;
        }

        inline Component *Instantiate(GameObject *pGameObject) const
        {
            return mComponentInstantiator(pGameObject);
        }

    private:
        const ComponentType &mrComponentType;
        ComponentInstantiator mComponentInstantiator;
    };

    class ComponentRegistry
    {
    public:
        inline static const ComponentRegister *GetComponentRegister(const std::string &rComponentName)
        {
            auto &rComponentRegisters = GetComponentRegisters();
            auto it = std::find_if(rComponentRegisters.cbegin(), rComponentRegisters.cend(), [&rComponentName](const auto &rComponentRegister)
                                   { return rComponentRegister.GetType().GetName() == rComponentName; });
            if (it == rComponentRegisters.cend())
            {
                return nullptr;
            }
            return &(*it);
        }

        template <typename ComponentT>
        inline static void RegisterComponent()
        {
            auto &rComponentRegisters = GetComponentRegisters();
            auto it = std::find_if(rComponentRegisters.cbegin(), rComponentRegisters.cend(), [](const auto &rComponentRegister)
                                   { return rComponentRegister.RefersTo<ComponentT>(); });
            assert(it == rComponentRegisters.cend());
            rComponentRegisters.emplace_back(ComponentT::TYPE, &ComponentT::GenericInstantiate);
        }

    private:
        ComponentRegistry() = delete;
        ~ComponentRegistry() = delete;

        static std::vector<ComponentRegister> &GetComponentRegisters()
        {
            static std::vector<ComponentRegister> sComponentsInstantiators;
            return sComponentsInstantiators;
        }
    };

}

#endif