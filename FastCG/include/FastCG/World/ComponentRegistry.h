#ifndef FASTCG_COMPONENT_REGISTRY_H
#define FASTCG_COMPONENT_REGISTRY_H

#include <FastCG/World/Component.h>

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

namespace FastCG
{
    using ComponentInstantiator = Component *(*)(GameObject *);

    class ComponentRegister
    {
    public:
        ComponentRegister(const ComponentType &rComponentType, ComponentInstantiator componentInstantiator)
            : mrComponentType(rComponentType), mComponentInstantiator(componentInstantiator)
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
            auto &rComponentRegisters = GetOrCreateComponentRegisters();
            auto it = std::find_if(rComponentRegisters.cbegin(), rComponentRegisters.cend(),
                                   [&rComponentName](const auto &rComponentRegister) {
                                       return rComponentRegister.GetType().GetName() == rComponentName;
                                   });
            if (it == rComponentRegisters.cend())
            {
                return nullptr;
            }
            return &(*it);
        }

        template <typename ComponentT>
        inline static void RegisterComponent()
        {
            auto &rComponentRegisters = GetOrCreateComponentRegisters();
            for (auto &rComponentRegister : rComponentRegisters)
            {
                if (rComponentRegister.RefersTo<ComponentT>())
                {
                    assert(false);
                }
            }
            rComponentRegisters.emplace_back(ComponentT::TYPE, &ComponentT::GenericInstantiate);
        }

        inline static const std::vector<ComponentRegister> &GetComponentRegisters()
        {
            return GetOrCreateComponentRegisters();
        }

    private:
        ComponentRegistry() = delete;
        ~ComponentRegistry() = delete;

        static std::vector<ComponentRegister> &GetOrCreateComponentRegisters()
        {
            static std::vector<ComponentRegister> sComponentsRegisters;
            return sComponentsRegisters;
        }
    };

}

#endif