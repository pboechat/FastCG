#include <FastCG/WorldLoader.h>
#include <FastCG/Transform.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>
#include <FastCG/ComponentRegistry.h>
#include <FastCG/Component.h>

#ifdef FASTCG_LINUX
// X11 defines Bool and rapidjson uses Bool as a member-function identifier
#ifdef Bool
#undef Bool
#endif
#endif
#include <rapidjson/document.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <cassert>

namespace
{
    template <typename GenericObjectT>
    void LoadInspectable(FastCG::Inspectable *, const GenericObjectT &);

    template <typename GenericValueT>
    void LoadInspectableProperty(FastCG::IInspectableProperty *pInspectableProperty, const GenericValueT &rGenericValue)
    {
        switch (pInspectableProperty->GetType())
        {
        case FastCG::InspectablePropertyType::INSPECTABLE:
        {
            FastCG::Inspectable *pInspectable;
            pInspectableProperty->GetValue(&pInspectable);
            LoadInspectable(pInspectable, rGenericValue.GetObj());
        }
        break;
        case FastCG::InspectablePropertyType::ENUM:
        {
            auto pInspectableEnumProperty = static_cast<FastCG::IInspectableEnumProperty *>(pInspectableProperty);
            const auto *pValue = rGenericValue.GetString();
            pInspectableEnumProperty->SetSelectedItem(pValue);
        }
        break;
        case FastCG::InspectablePropertyType::BOOL:
        {
            auto value = rGenericValue.GetBool();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::STRING:
        {
            std::string value = rGenericValue.GetString();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::INT32:
        {
            auto value = rGenericValue.GetInt();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::UINT32:
        {
            auto value = rGenericValue.GetUint();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::INT64:
        {
            auto value = rGenericValue.GetInt64();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::UINT64:
        {
            auto value = rGenericValue.GetUint64();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::FLOAT:
        {
            auto value = rGenericValue.GetFloat();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::DOUBLE:
        {
            auto value = rGenericValue.GetDouble();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC2:
        {
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 2);
            auto value = glm::vec2{array[0].GetFloat(), array[1].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC3:
        {
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 3);
            auto value = glm::vec3{array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC4:
        {
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 4);
            auto value = glm::vec4{array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat(), array[3].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        default:
            assert(false);
        }
    }

    template <typename GenericObjectT>
    void LoadInspectable(FastCG::Inspectable *pInspectable, const GenericObjectT &rInspectableObj)
    {
        for (auto it = rInspectableObj.MemberBegin(); it != rInspectableObj.MemberEnd(); ++it)
        {
            std::string propertyName = it->name.GetString();
            auto *pInspectableProperty = pInspectable->GetInspectableProperty(propertyName);
            // ignore non-existing inspectable properties
            if (pInspectableProperty == nullptr)
            {
                continue;
            }
            LoadInspectableProperty(pInspectableProperty, it->value);
        }
    }

    template <typename GenericObjectT>
    void LoadGameObject(FastCG::GameObject *pParent, const GenericObjectT &rGameObjectObj)
    {
        assert(rGameObjectObj.HasMember("name") && rGameObjectObj["name"].IsString());
        std::string name = rGameObjectObj["name"].GetString();

        auto scale = glm::vec3{1, 1, 1};
        auto rotation = glm::quat{};
        auto position = glm::vec3{0, 0, 0};
        if (rGameObjectObj.HasMember("transform"))
        {
            assert(rGameObjectObj["transform"].IsObject());
            auto transformObj = rGameObjectObj["transform"].GetObj();
            if (transformObj.HasMember("scale"))
            {
                assert(transformObj["scale"].IsArray());
                auto scaleArray = transformObj["scale"].GetArray();
                assert(scaleArray.Size() == 3);
                scale = glm::vec3{scaleArray[0].GetFloat(), scaleArray[1].GetFloat(), scaleArray[2].GetFloat()};
            }
            if (transformObj.HasMember("rotation"))
            {
                assert(transformObj["rotation"].IsArray());
                auto rotationArray = transformObj["rotation"].GetArray();
                assert(rotationArray.Size() == 4);
                rotation = glm::quat{rotationArray[0].GetFloat(), rotationArray[1].GetFloat(), rotationArray[2].GetFloat(), rotationArray[3].GetFloat()};
            }
            if (transformObj.HasMember("position"))
            {
                assert(transformObj["position"].IsArray());
                auto positionArray = transformObj["position"].GetArray();
                assert(positionArray.Size() == 3);
                position = glm::vec3{positionArray[0].GetFloat(), positionArray[1].GetFloat(), positionArray[2].GetFloat()};
            }
        }

        auto *pGameObject = FastCG::GameObject::Instantiate(name, scale, rotation, position);

        pGameObject->GetTransform()->SetParent(pParent->GetTransform());

        if (rGameObjectObj.HasMember("components"))
        {
            assert(rGameObjectObj["components"].IsArray());
            auto componentsArray = rGameObjectObj["components"].GetArray();
            for (auto &rComponentArrayEl : componentsArray)
            {
                assert(rComponentArrayEl.IsObject());
                auto componentObj = rComponentArrayEl.GetObj();
                assert(componentObj.HasMember("name") && componentObj["name"].IsString());
                std::string componentName = componentObj["name"].GetString();
                const auto *pComponentRegistry = FastCG::ComponentRegistry::GetComponentRegister(componentName.c_str());
                // ignore non-registered/non-existing components
                if (pComponentRegistry != nullptr)
                {
                    auto *pComponent = pComponentRegistry->Instantiate(pGameObject);
                    LoadInspectable(pComponent, componentObj);
                }
            }
        }

        if (rGameObjectObj.HasMember("children"))
        {
            assert(rGameObjectObj["children"].IsArray());
            auto childrenArray = rGameObjectObj["children"].GetArray();
            for (auto &rChildEl : childrenArray)
            {
                assert(rChildEl.IsObject());
                auto childGameObjectObj = rChildEl.GetObj();
                LoadGameObject(pGameObject, childGameObjectObj);
            }
        }
    }
}

namespace FastCG
{
    GameObject *WorldLoader::Load(const std::string &rFilePath)
    {
        size_t size;
        auto data = FileReader::ReadText(rFilePath, size);
        std::string jsonStr(data.get(), data.get() + size);

        rapidjson::Document document;
        document.Parse(jsonStr.c_str());

        assert(document.IsArray());

        std::string worldName = File::GetFileNameWithoutExtension(rFilePath);
        auto *pWorldRoot = GameObject::Instantiate(worldName);

        auto gameObjectsArray = document.GetArray();
        for (auto &rGameObjectEl : gameObjectsArray)
        {
            assert(rGameObjectEl.IsObject());
            auto gameObjectObj = rGameObjectEl.GetObj();
            LoadGameObject(pWorldRoot, gameObjectObj);
        }

        return pWorldRoot;
    }

    std::string WorldLoader::Dump(GameObject *pWorldRoot)
    {
        // TODO:
        return {};
    }

}