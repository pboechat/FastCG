#include <FastCG/GameObjectLoader.h>
#include <FastCG/Transform.h>
#include <FastCG/RenderingEnums.h>
#include <FastCG/Inspectable.h>
#include <FastCG/GraphicsSystem.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>
#include <FastCG/FastCG.h>
#include <FastCG/ComponentRegistry.h>
#include <FastCG/Component.h>
#include <FastCG/Base64.h>

#ifdef FASTCG_LINUX
// X11 defines Bool and rapidjson uses Bool as a member-function identifier
#ifdef Bool
#undef Bool
#endif
#endif
#include <rapidjson/document.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <cassert>
#include <array>

namespace
{
    constexpr size_t MAX_MESH_BUFFERS = 16;

    template <typename GenericValueT, typename EnumT>
    void GetValue(const GenericValueT &rGenericValue, EnumT &rValue, const char *const *pValues, size_t valueCount)
    {
        static_assert(std::is_enum_v<EnumT>, "EnumT must be an enumerator");
        assert(rGenericValue.IsString());
        std::string valueStr = rGenericValue.GetString();
        auto pValuesEnd = pValues + valueCount;
        auto it = std::find(pValues, pValuesEnd, valueStr);
        assert(it != pValuesEnd);
        rValue = (EnumT)std::distance(pValues, it);
    }

    template <typename GenericObjectT>
    void LoadInspectable(const GenericObjectT &,
                         const std::unordered_map<std::string, FastCG::Material *> &,
                         const std::unordered_map<std::string, FastCG::Mesh *> &,
                         const std::unordered_map<std::string, FastCG::Texture *> &,
                         FastCG::Inspectable *);

    template <typename GenericObjectT>
    FastCG::Material *LoadMaterial(const GenericObjectT &rGenericObj, const std::string &rBasePath, const std::unordered_map<std::string, FastCG::Texture *> &rTextures)
    {
        FastCG::MaterialArgs args{};
        assert(rGenericObj.HasMember("name") && rGenericObj["name"].IsString());
        args.name = rGenericObj["name"].GetString();
        {
            assert(rGenericObj.HasMember("materialDefinition") && rGenericObj["materialDefinition"].IsString());
            std::string materialDefinitionName = rGenericObj["materialDefinition"].GetString();
            args.pMaterialDefinition = FastCG::GraphicsSystem::GetInstance()->FindMaterialDefinition(materialDefinitionName);
        }
        assert(args.pMaterialDefinition != nullptr);
        std::string constantBufferData;
        if (rGenericObj.HasMember("constantBufferData"))
        {
            assert(rGenericObj["constantBufferData"].IsString());
            constantBufferData = FastCG::DecodeBase64(rGenericObj["constantBufferData"].GetString());
        }
        std::unordered_map<std::string, const FastCG::Texture *> textures;
        if (rGenericObj.HasMember("textures"))
        {
            assert(rGenericObj["textures"].IsArray());
            auto texturesArray = rGenericObj["textures"].GetArray();
            for (auto &rRefValue : texturesArray)
            {
                assert(rRefValue.IsObject());
                auto refObj = rRefValue.GetObj();
                assert(refObj.HasMember("name") && refObj["name"].IsString());
                std::string name = refObj["name"].GetString();
                assert(refObj.HasMember("id") && refObj["id"].IsString());
                std::string id = refObj["id"].GetString();
                auto it = rTextures.find(id);
                assert(it != rTextures.end());
                textures[name] = it->second;
            }
        }
        auto *pMaterial = FastCG::GraphicsSystem::GetInstance()->CreateMaterial(args);
        pMaterial->SetConstantBufferData(reinterpret_cast<const uint8_t *>(constantBufferData.data()), 0, constantBufferData.size());
        for (auto it = textures.begin(); it != textures.end(); ++it)
        {
            pMaterial->SetTexture(it->first, it->second);
        }
        return pMaterial;
    }

    template <typename GenericObjectT>
    FastCG::Mesh *LoadMesh(const GenericObjectT &rGenericObj, const std::string &rBasePath)
    {
        FastCG::MeshArgs args{};
        assert(rGenericObj.HasMember("name") && rGenericObj["name"].IsString());
        args.name = rGenericObj["name"].GetString();
        std::array<std::unique_ptr<uint8_t[]>, MAX_MESH_BUFFERS> buffersData;
        size_t bufferIdx = 0;
        if (rGenericObj.HasMember("vertexBuffers"))
        {
            assert(rGenericObj["vertexBuffers"].IsArray());
            auto vertexBuffersArray = rGenericObj["vertexBuffers"].GetArray();
            args.vertexAttributeDecriptors.resize((size_t)vertexBuffersArray.Size());
            for (size_t i = 0; i < args.vertexAttributeDecriptors.size(); ++i)
            {
                auto &rVertexAttributeDescriptor = args.vertexAttributeDecriptors[i];
                auto &rVertexBufferValue = vertexBuffersArray[(rapidjson::SizeType)i];
                assert(rVertexBufferValue.IsObject());
                auto vertexBufferObj = rVertexBufferValue.GetObj();
                assert(vertexBufferObj.HasMember("name") && vertexBufferObj["name"].IsString());
                rVertexAttributeDescriptor.name = vertexBufferObj["name"].GetString();
                assert(vertexBufferObj.HasMember("usage") && vertexBufferObj["usage"].IsString());
                GetValue(vertexBufferObj["usage"], rVertexAttributeDescriptor.usage, FastCG::BUFFER_USAGE_STRINGS, FASTCG_ARRAYSIZE(FastCG::BUFFER_USAGE_STRINGS));
                if (vertexBufferObj.HasMember("dataPath"))
                {
                    assert(vertexBufferObj["dataPath"].IsString());
                    auto filePath = FastCG::File::Join({rBasePath, vertexBufferObj["dataPath"].GetString()});
                    buffersData[bufferIdx] = FastCG::FileReader::ReadBinary(filePath, rVertexAttributeDescriptor.dataSize);
                }
                else
                {
                    assert(vertexBufferObj.HasMember("dataSize") && vertexBufferObj["dataSize"].IsUint64());
                    rVertexAttributeDescriptor.dataSize = vertexBufferObj["dataSize"].GetUint64();
                    assert(vertexBufferObj.HasMember("data") && vertexBufferObj["data"].IsString());
                    auto data = FastCG::DecodeBase64(vertexBufferObj["data"].GetString());
                    auto bufferData = std::make_unique<uint8_t[]>(data.size());
                    memcpy((void *)bufferData.get(), (const void *)data.data(), data.size());
                    buffersData[bufferIdx] = std::move(bufferData);
                }
                rVertexAttributeDescriptor.pData = (const void *)buffersData[bufferIdx++].get();
                assert(vertexBufferObj.HasMember("bindingDescriptors") && vertexBufferObj["bindingDescriptors"].IsArray());
                auto bindingDescriptorsArray = vertexBufferObj["bindingDescriptors"].GetArray();
                rVertexAttributeDescriptor.bindingDescriptors.resize((size_t)bindingDescriptorsArray.Size());
                for (decltype(bindingDescriptorsArray.Size()) j = 0; j < bindingDescriptorsArray.Size(); ++j)
                {
                    auto &rBindingDescriptorValue = bindingDescriptorsArray[j];
                    auto &rBindingDescriptor = rVertexAttributeDescriptor.bindingDescriptors[j];
                    assert(rBindingDescriptorValue.IsObject());
                    auto bindingDescriptorObj = rBindingDescriptorValue.GetObj();
                    assert(bindingDescriptorObj.HasMember("binding") && bindingDescriptorObj["binding"].IsUint());
                    rBindingDescriptor.binding = bindingDescriptorObj["binding"].GetUint();
                    assert(bindingDescriptorObj.HasMember("size") && bindingDescriptorObj["size"].IsUint());
                    rBindingDescriptor.size = bindingDescriptorObj["size"].GetUint();
                    assert(bindingDescriptorObj.HasMember("type") && bindingDescriptorObj["type"].IsString());
                    GetValue(bindingDescriptorObj["type"], rBindingDescriptor.type, FastCG::VERTEX_DATA_TYPE_STRINGS, FASTCG_ARRAYSIZE(FastCG::VERTEX_DATA_TYPE_STRINGS));
                    assert(bindingDescriptorObj.HasMember("normalized") && bindingDescriptorObj["normalized"].IsBool());
                    rBindingDescriptor.normalized = bindingDescriptorObj["normalized"].GetBool();
                    assert(bindingDescriptorObj.HasMember("stride") && bindingDescriptorObj["stride"].IsUint());
                    rBindingDescriptor.stride = bindingDescriptorObj["stride"].GetUint();
                    assert(bindingDescriptorObj.HasMember("offset") && bindingDescriptorObj["offset"].IsUint());
                    rBindingDescriptor.offset = bindingDescriptorObj["offset"].GetUint();
                }
            }
        }
        assert(rGenericObj.HasMember("indexBuffer") && rGenericObj["indexBuffer"].IsObject());
        auto indexBufferObj = rGenericObj["indexBuffer"].GetObj();
        assert(indexBufferObj.HasMember("usage") && indexBufferObj["usage"].IsString());
        GetValue(indexBufferObj["usage"], args.indices.usage, FastCG::BUFFER_USAGE_STRINGS, FASTCG_ARRAYSIZE(FastCG::BUFFER_USAGE_STRINGS));
        if (indexBufferObj.HasMember("dataPath"))
        {
            assert(indexBufferObj["dataPath"].IsString());
            auto filePath = FastCG::File::Join({rBasePath, indexBufferObj["dataPath"].GetString()});
            size_t dataSize;
            buffersData[bufferIdx] = FastCG::FileReader::ReadBinary(filePath, dataSize);
            assert(dataSize % sizeof(uint32_t) == 0);
            args.indices.count = (uint32_t)(dataSize / sizeof(uint32_t));
        }
        else
        {
            assert(indexBufferObj.HasMember("count") && indexBufferObj["count"].IsUint());
            args.indices.count = indexBufferObj["count"].GetUint();
            assert(indexBufferObj.HasMember("data") && indexBufferObj["data"].IsString());
            auto data = FastCG::DecodeBase64(indexBufferObj["data"].GetString());
            auto bufferData = std::make_unique<uint8_t[]>(data.size());
            memcpy((void *)bufferData.get(), (const void *)data.data(), data.size());
            buffersData[bufferIdx] = std::move(bufferData);
        }
        args.indices.pData = (const uint32_t *)buffersData[bufferIdx++].get();
        if (rGenericObj.HasMember("bounds"))
        {
            assert(rGenericObj["bounds"].IsObject());
            auto boundsObj = rGenericObj["bounds"].GetObj();
            assert(boundsObj.HasMember("min") && boundsObj["min"].IsArray());
            auto minArray = boundsObj["min"].GetArray();
            assert(minArray.Size() == 3);
            args.bounds.min = {minArray[0].GetFloat(), minArray[1].GetFloat(), minArray[2].GetFloat()};
            assert(boundsObj.HasMember("max") && boundsObj["max"].IsArray());
            auto maxArray = boundsObj["min"].GetArray();
            assert(maxArray.Size() == 3);
            args.bounds.max = {maxArray[0].GetFloat(), maxArray[1].GetFloat(), maxArray[2].GetFloat()};
        }
        auto *pMesh = FastCG::GraphicsSystem::GetInstance()->CreateMesh(args);
        return pMesh;
    }

    template <typename GenericObjectT>
    FastCG::Texture *LoadTexture(const GenericObjectT &rGenericObj, const std::string &rBasePath)
    {
        FastCG::TextureArgs args{};
        assert(rGenericObj.HasMember("name") && rGenericObj["name"].IsString());
        args.name = rGenericObj["name"].GetString();
        assert(rGenericObj.HasMember("width") && rGenericObj["width"].IsUint());
        args.width = rGenericObj["width"].GetUint();
        assert(rGenericObj.HasMember("height") && rGenericObj["height"].IsUint());
        args.height = rGenericObj["height"].GetUint();
        assert(rGenericObj.HasMember("type") && rGenericObj["type"].IsString());
        GetValue(rGenericObj["type"], args.type, FastCG::TEXTURE_TYPE_STRINGS, FASTCG_ARRAYSIZE(FastCG::TEXTURE_TYPE_STRINGS));
        assert(rGenericObj.HasMember("format") && rGenericObj["format"].IsString());
        GetValue(rGenericObj["format"], args.format, FastCG::TEXTURE_FORMAT_STRINGS, FASTCG_ARRAYSIZE(FastCG::TEXTURE_FORMAT_STRINGS));
        assert(rGenericObj.HasMember("bitsPerPixel") && rGenericObj["bitsPerPixel"].IsArray());
        auto bitsPerPixelArray = rGenericObj["bitsPerPixel"].GetArray();
        assert(bitsPerPixelArray.Size() == 4);
        args.bitsPerPixel = FastCG::BitsPerPixel{(uint8_t)bitsPerPixelArray[0].GetUint(), (uint8_t)bitsPerPixelArray[1].GetUint(), (uint8_t)bitsPerPixelArray[2].GetUint(), (uint8_t)bitsPerPixelArray[3].GetUint()};
        assert(rGenericObj.HasMember("dataType") && rGenericObj["dataType"].IsString());
        GetValue(rGenericObj["dataType"], args.dataType, FastCG::TEXTURE_DATA_TYPE_STRINGS, FASTCG_ARRAYSIZE(FastCG::TEXTURE_DATA_TYPE_STRINGS));
        assert(rGenericObj.HasMember("filter") && rGenericObj["filter"].IsString());
        GetValue(rGenericObj["filter"], args.filter, FastCG::TEXTURE_FILTER_STRINGS, FASTCG_ARRAYSIZE(FastCG::TEXTURE_FILTER_STRINGS));
        assert(rGenericObj.HasMember("wrapMode") && rGenericObj["wrapMode"].IsString());
        GetValue(rGenericObj["wrapMode"], args.wrapMode, FastCG::TEXTURE_WRAP_MODE_STRINGS, FASTCG_ARRAYSIZE(FastCG::TEXTURE_WRAP_MODE_STRINGS));
        if (rGenericObj.HasMember("generateMipmap"))
        {
            assert(rGenericObj["generateMipmap"].IsBool());
            args.generateMipmap = rGenericObj["generateMipmap"].GetBool();
        }
        FastCG::Texture *pTexture;
        if (rGenericObj.HasMember("dataPath"))
        {
            assert(rGenericObj["dataPath"].IsString());
            auto filePath = FastCG::File::Join({rBasePath, rGenericObj["dataPath"].GetString()});
            size_t dataSize;
            auto data = FastCG::FileReader::ReadBinary(filePath, dataSize);
            args.pData = (const void *)data.get();
            pTexture = FastCG::GraphicsSystem::GetInstance()->CreateTexture(args);
        }
        else
        {
            assert(rGenericObj.HasMember("data") && rGenericObj["data"].IsString());
            auto data = FastCG::DecodeBase64(rGenericObj["data"].GetString());
            args.pData = (const void *)data.data();
            pTexture = FastCG::GraphicsSystem::GetInstance()->CreateTexture(args);
        }
        return pTexture;
    }

    template <typename GenericValueT>
    void LoadInspectableProperty(const GenericValueT &rGenericValue,
                                 const std::string &rBasePath,
                                 const std::unordered_map<std::string, FastCG::Material *> &rMaterials,
                                 const std::unordered_map<std::string, FastCG::Mesh *> &rMeshes,
                                 const std::unordered_map<std::string, FastCG::Texture *> &rTextures,
                                 FastCG::IInspectableProperty *pInspectableProperty)
    {
        switch (pInspectableProperty->GetType())
        {
        case FastCG::InspectablePropertyType::INSPECTABLE:
        {
            assert(rGenericValue.IsObject());
            FastCG::Inspectable *pInspectable;
            pInspectableProperty->GetValue(&pInspectable);
            LoadInspectable(rGenericValue.GetObj(), rBasePath, rMaterials, rMeshes, rTextures, pInspectable);
        }
        break;
        case FastCG::InspectablePropertyType::ENUM:
        {
            assert(rGenericValue.IsString());
            auto *pInspectableEnumProperty = static_cast<FastCG::IInspectableEnumProperty *>(pInspectableProperty);
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
            assert(rGenericValue.IsArray());
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 2);
            auto value = glm::vec2{array[0].GetFloat(), array[1].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC3:
        {
            assert(rGenericValue.IsArray());
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 3);
            auto value = glm::vec3{array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC4:
        {
            assert(rGenericValue.IsArray());
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 4);
            auto value = glm::vec4{array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat(), array[3].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::STRING:
        {
            assert(rGenericValue.IsString());
            std::string value = rGenericValue.GetString();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::MATERIAL:
        {
            assert(rGenericValue.IsString());
            std::string id = rGenericValue.GetString();
            auto it = rMaterials.find(id);
            assert(it != rMaterials.end());
            pInspectableProperty->SetValue(&it->second);
        }
        break;
        case FastCG::InspectablePropertyType::MESH:
        {
            assert(rGenericValue.IsString());
            std::string id = rGenericValue.GetString();
            auto it = rMeshes.find(id);
            assert(it != rMeshes.end());
            pInspectableProperty->SetValue(&it->second);
        }
        break;
        case FastCG::InspectablePropertyType::TEXTURE:
        {
            assert(rGenericValue.IsString());
            std::string id = rGenericValue.GetString();
            auto it = rTextures.find(id);
            assert(it != rTextures.end());
            pInspectableProperty->SetValue(&it->second);
        }
        break;
        default:
            assert(false);
        }
    }

    template <typename GenericObjectT>
    void LoadInspectable(const GenericObjectT &rInspectableObj,
                         const std::string &rBasePath,
                         const std::unordered_map<std::string, FastCG::Material *> &rMaterials,
                         const std::unordered_map<std::string, FastCG::Mesh *> &rMeshes,
                         const std::unordered_map<std::string, FastCG::Texture *> &rTextures,
                         FastCG::Inspectable *pInspectable)
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
            LoadInspectableProperty(it->value, rBasePath, rMaterials, rMeshes, rTextures, pInspectableProperty);
        }
    }

    template <typename GenericObjectT>
    FastCG::GameObject *LoadGameObject(const GenericObjectT &rGameObjectObj,
                                       const std::string &rBasePath,
                                       const std::unordered_map<std::string, FastCG::Material *> &rMaterials,
                                       const std::unordered_map<std::string, FastCG::Mesh *> &rMeshes,
                                       const std::unordered_map<std::string, FastCG::Texture *> &rTextures,
                                       FastCG::GameObject *pParent)
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
                auto w = rotationArray[0].GetFloat();
                auto x = rotationArray[1].GetFloat();
                auto y = rotationArray[2].GetFloat();
                auto z = rotationArray[3].GetFloat();
                rotation = glm::quat(w, x, y, z);
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
        if (pParent != nullptr)
        {
            pGameObject->GetTransform()->SetParent(pParent->GetTransform());
        }

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
                    LoadInspectable(componentObj, rBasePath, rMaterials, rMeshes, rTextures, pComponent);
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
                LoadGameObject(childGameObjectObj, rBasePath, rMaterials, rMeshes, rTextures, pGameObject);
            }
        }

        return pGameObject;
    }
}

namespace FastCG
{
    GameObject *GameObjectLoader::Load(const std::string &rFilePath, GameObjectoaderOptionMaskType options)
    {
        size_t size;
        auto data = FileReader::ReadText(rFilePath, size);
        if (size == 0)
        {
            return nullptr;
        }
        std::string jsonStr(data.get(), data.get() + size);

        auto basePath = File::GetBasePath(rFilePath);

        rapidjson::Document document;
        document.Parse(jsonStr.c_str());

        assert(document.IsArray());
        auto docArray = document.GetArray();
        assert(docArray.Size() == 1 && docArray[0].IsObject());
        auto docObj = docArray[0].GetObj();

        std::unordered_map<std::string, Texture *> textures;
        std::unordered_map<std::string, Material *> materials;
        std::unordered_map<std::string, Mesh *> meshes;
        if (docObj.HasMember("resources"))
        {
            assert(docObj["resources"].IsObject());
            auto resourcesObj = docObj["resources"].GetObj();
            if (resourcesObj.HasMember("textures"))
            {
                assert(resourcesObj["textures"].IsArray());
                for (auto &rRefValue : resourcesObj["textures"].GetArray())
                {
                    assert(rRefValue.IsObject());
                    auto refObj = rRefValue.GetObj();
                    assert(refObj.HasMember("id") && refObj["id"].IsString());
                    std::string id = refObj["id"].GetString();
                    assert(refObj.HasMember("value") && refObj["value"].IsObject());
                    auto *pTexture = LoadTexture(refObj["value"].GetObj(), basePath);
                    textures.emplace(id, pTexture);
                }
            }

            if (resourcesObj.HasMember("materials"))
            {
                assert(resourcesObj["materials"].IsArray());
                for (auto &rRefValue : resourcesObj["materials"].GetArray())
                {
                    assert(rRefValue.IsObject());
                    auto refObj = rRefValue.GetObj();
                    assert(refObj.HasMember("id") && refObj["id"].IsString());
                    std::string id = refObj["id"].GetString();
                    assert(refObj.HasMember("value") && refObj["value"].IsObject());
                    auto *pMaterial = LoadMaterial(refObj["value"].GetObj(), basePath, textures);
                    materials.emplace(id, pMaterial);
                }
            }

            if (resourcesObj.HasMember("meshes"))
            {
                assert(resourcesObj["meshes"].IsArray());
                for (auto &rRefValue : resourcesObj["meshes"].GetArray())
                {
                    assert(rRefValue.IsObject());
                    auto refObj = rRefValue.GetObj();
                    assert(refObj.HasMember("id") && refObj["id"].IsString());
                    std::string id = refObj["id"].GetString();
                    assert(refObj.HasMember("value") && refObj["value"].IsObject());
                    auto meshObj = refObj["value"].GetObj();
                    auto *pMesh = LoadMesh(meshObj, basePath);
                    meshes.emplace(id, pMesh);
                }
            }
        }

        GameObject *pRoot = nullptr;
        if (docObj.HasMember("root"))
        {
            assert(docObj["root"].IsObject());
            auto worldObj = docObj["root"].GetObj();
            pRoot = LoadGameObject(worldObj, basePath, materials, meshes, textures, nullptr);
        }

        return pRoot;
    }

}