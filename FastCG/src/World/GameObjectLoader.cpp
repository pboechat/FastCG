#include <FastCG/Core/Base64.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Platform/FileReader.h>
#include <FastCG/Reflection/Inspectable.h>
#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/World/Component.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/World/GameObjectLoader.h>
#include <FastCG/World/Transform.h>

#ifdef FASTCG_LINUX
// X11 defines Bool and rapidjson uses Bool as a member-function identifier
#ifdef Bool
#undef Bool
#endif
#endif
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <rapidjson/document.h>

#include <array>
#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace
{
    constexpr size_t MAX_MESH_BUFFERS = 16;

    template <typename GenericValueT, typename EnumT>
    void GetValue(const GenericValueT &rGenericValue, EnumT &rValue, const char *const *pValues, size_t valueCount)
    {
        static_assert(std::is_enum<EnumT>::value, "EnumT must be an enumerator");
        assert(rGenericValue.IsString());
        std::string valueStr = rGenericValue.GetString();
        auto pValuesEnd = pValues + valueCount;
        auto it = std::find(pValues, pValuesEnd, valueStr);
        assert(it != pValuesEnd);
        rValue = (EnumT)std::distance(pValues, it);
    }

    template <typename GenericObjectT>
    std::unique_ptr<FastCG::Material> LoadMaterial(const GenericObjectT &rGenericObj,
                                                   const std::unordered_map<std::string, FastCG::Texture *> &rTextures)
    {
        FastCG::MaterialArgs args{};
        assert(rGenericObj.HasMember("name") && rGenericObj["name"].IsString());
        args.name = rGenericObj["name"].GetString();
        {
            assert(rGenericObj.HasMember("materialDefinition") && rGenericObj["materialDefinition"].IsString());
            std::string materialDefinitionName = rGenericObj["materialDefinition"].GetString();
            args.pMaterialDefinition =
                FastCG::MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition(materialDefinitionName);
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
        auto pMaterial = std::make_unique<FastCG::Material>(args);
        pMaterial->SetConstantBufferData(reinterpret_cast<const uint8_t *>(constantBufferData.data()), 0,
                                         constantBufferData.size());
        for (auto it = textures.begin(); it != textures.end(); ++it)
        {
            pMaterial->SetTexture(it->first, it->second);
        }
        return pMaterial;
    }

    template <typename GenericObjectT>
    std::unique_ptr<FastCG::Mesh> LoadMesh(const GenericObjectT &rGenericObj, const std::filesystem::path &rBasePath)
    {
        FastCG::MeshArgs args{};
        assert(rGenericObj.HasMember("name") && rGenericObj["name"].IsString());
        args.name = rGenericObj["name"].GetString();
        std::array<std::unique_ptr<uint8_t[]>, MAX_MESH_BUFFERS> buffersData;
        std::unique_ptr<uint32_t[]> indexBufferData;
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
                assert(vertexBufferObj.HasMember("usage") && vertexBufferObj["usage"].IsUint());
                rVertexAttributeDescriptor.usage = (FastCG::BufferUsageFlags)vertexBufferObj["usage"].GetUint();
                if (vertexBufferObj.HasMember("encodedData"))
                {
                    assert(vertexBufferObj["encodedData"].IsString());
                    auto dataStr = FastCG::DecodeBase64(vertexBufferObj["encodedData"].GetString());
                    rVertexAttributeDescriptor.dataSize = dataStr.size();
                    auto bufferData = std::make_unique<uint8_t[]>(dataStr.size());
                    std::memcpy((void *)bufferData.get(), (const void *)dataStr.data(), dataStr.size());
                    buffersData[bufferIdx] = std::move(bufferData);
                }
                else
                {
                    assert(vertexBufferObj.HasMember("data") && vertexBufferObj["data"].IsArray());
                    auto dataArray = vertexBufferObj["data"].GetArray();
                    rVertexAttributeDescriptor.dataSize = dataArray.Size() * sizeof(float);
                    auto pFloats = std::make_unique<float[]>(dataArray.Size());
                    for (decltype(dataArray.Size()) j = 0; j < dataArray.Size(); ++j)
                    {
                        auto &rDataElement = dataArray[j];
                        assert(rDataElement.IsFloat());
                        pFloats[j] = rDataElement.GetFloat();
                    }
                    buffersData[bufferIdx] = std::unique_ptr<uint8_t[]>(reinterpret_cast<uint8_t *>(pFloats.release()));
                }
                rVertexAttributeDescriptor.pData = (const void *)buffersData[bufferIdx++].get();
                assert(vertexBufferObj.HasMember("bindingDescriptors") &&
                       vertexBufferObj["bindingDescriptors"].IsArray());
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
                    GetValue(bindingDescriptorObj["type"], rBindingDescriptor.type, FastCG::VertexDataType_STRINGS,
                             FASTCG_ARRAYSIZE(FastCG::VertexDataType_STRINGS));
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
        assert(indexBufferObj.HasMember("usage") && indexBufferObj["usage"].IsUint());
        args.indices.usage = (FastCG::BufferUsageFlags)indexBufferObj["usage"].GetUint();
        if (indexBufferObj.HasMember("encodedData"))
        {
            assert(indexBufferObj.HasMember("encodedData") && indexBufferObj["encodedData"].IsString());
            auto dataStr = FastCG::DecodeBase64(indexBufferObj["encodedData"].GetString());
            assert(dataStr.size() % sizeof(uint32_t) == 0);
            args.indices.count = (uint32_t)(dataStr.size() / sizeof(uint32_t));
            indexBufferData = std::make_unique<uint32_t[]>(args.indices.count);
            std::memcpy((void *)indexBufferData.get(), (const void *)dataStr.data(), dataStr.size());
        }
        else
        {
            assert(indexBufferObj.HasMember("data") && indexBufferObj["data"].IsArray());
            auto dataArray = indexBufferObj["data"].GetArray();
            args.indices.count = (uint32_t)dataArray.Size();
            indexBufferData = std::make_unique<uint32_t[]>(args.indices.count);
            for (decltype(dataArray.Size()) j = 0; j < dataArray.Size(); ++j)
            {
                auto &rDataElement = dataArray[j];
                assert(rDataElement.IsUint());
                indexBufferData[j] = rDataElement.GetUint();
            }
        }
        args.indices.pData = (const uint32_t *)indexBufferData.get();
        if (rGenericObj.HasMember("bounds"))
        {
            assert(rGenericObj["bounds"].IsObject());
            auto boundsObj = rGenericObj["bounds"].GetObj();
            assert(boundsObj.HasMember("min") && boundsObj["min"].IsArray());
            auto minArray = boundsObj["min"].GetArray();
            assert(minArray.Size() == 3);
            args.bounds.min = glm::vec3{minArray[0].GetFloat(), minArray[1].GetFloat(), minArray[2].GetFloat()};
            assert(boundsObj.HasMember("max") && boundsObj["max"].IsArray());
            auto maxArray = boundsObj["min"].GetArray();
            assert(maxArray.Size() == 3);
            args.bounds.max = glm::vec3{maxArray[0].GetFloat(), maxArray[1].GetFloat(), maxArray[2].GetFloat()};
        }
        return std::make_unique<FastCG::Mesh>(args);
    }

    template <typename GenericObjectT>
    FastCG::Texture *LoadTexture(const GenericObjectT &rGenericObj, const std::filesystem::path &rBasePath)
    {
        if (rGenericObj.HasMember("path"))
        {
            assert(rGenericObj["path"].IsString());
            FastCG::TextureLoadSettings loadSettings;
            // TODO: use strings instead of numbers
            assert(rGenericObj.HasMember("usage") && rGenericObj["usage"].IsUint());
            loadSettings.usage = (FastCG::TextureUsageFlags)rGenericObj["usage"].GetUint();
            assert(rGenericObj.HasMember("filter") && rGenericObj["filter"].IsString());
            GetValue(rGenericObj["filter"], loadSettings.filter, FastCG::TextureFilter_STRINGS,
                     FASTCG_ARRAYSIZE(FastCG::TextureFilter_STRINGS));
            assert(rGenericObj.HasMember("wrapMode") && rGenericObj["wrapMode"].IsString());
            GetValue(rGenericObj["wrapMode"], loadSettings.wrapMode, FastCG::TextureWrapMode_STRINGS,
                     FASTCG_ARRAYSIZE(FastCG::TextureWrapMode_STRINGS));
            auto filePath = rBasePath / rGenericObj["path"].GetString();
            return FastCG::TextureLoader::Load(filePath, loadSettings);
        }
        else
        {
            FastCG::Texture::Args args{};
            assert(rGenericObj.HasMember("name") && rGenericObj["name"].IsString());
            args.name = rGenericObj["name"].GetString();
            assert(rGenericObj.HasMember("width") && rGenericObj["width"].IsUint());
            args.width = rGenericObj["width"].GetUint();
            assert(rGenericObj.HasMember("height") && rGenericObj["height"].IsUint());
            args.height = rGenericObj["height"].GetUint();
            assert(rGenericObj.HasMember("type") && rGenericObj["type"].IsString());
            GetValue(rGenericObj["type"], args.type, FastCG::TextureType_STRINGS,
                     FASTCG_ARRAYSIZE(FastCG::TextureType_STRINGS));
            // TODO: use strings instead of numbers
            assert(rGenericObj.HasMember("usage") && rGenericObj["usage"].IsUint());
            args.usage = (FastCG::TextureUsageFlags)rGenericObj["usage"].GetUint();
            assert(rGenericObj.HasMember("format") && rGenericObj["format"].IsString());
            GetValue(rGenericObj["format"], args.format, FastCG::TextureFormat_STRINGS,
                     FASTCG_ARRAYSIZE(FastCG::TextureFormat_STRINGS));
            assert(rGenericObj.HasMember("filter") && rGenericObj["filter"].IsString());
            GetValue(rGenericObj["filter"], args.filter, FastCG::TextureFilter_STRINGS,
                     FASTCG_ARRAYSIZE(FastCG::TextureFilter_STRINGS));
            assert(rGenericObj.HasMember("wrapMode") && rGenericObj["wrapMode"].IsString());
            GetValue(rGenericObj["wrapMode"], args.wrapMode, FastCG::TextureWrapMode_STRINGS,
                     FASTCG_ARRAYSIZE(FastCG::TextureWrapMode_STRINGS));
            assert(rGenericObj.HasMember("data") && rGenericObj["data"].IsString());
            auto data = FastCG::DecodeBase64(rGenericObj["data"].GetString());
            args.pData = reinterpret_cast<const uint8_t *>(data.data());
            return FastCG::GraphicsSystem::GetInstance()->CreateTexture(args);
        }
    }

    template <typename GenericObjectT>
    void LoadInspectable(const GenericObjectT &, const std::filesystem::path &,
                         const std::unordered_map<std::string, std::shared_ptr<FastCG::Material>> &,
                         const std::unordered_map<std::string, std::shared_ptr<FastCG::Mesh>> &,
                         const std::unordered_map<std::string, FastCG::Texture *> &, FastCG::Inspectable *,
                         const std::vector<std::string> &rPropertiesToIgnore = {});

    template <typename GenericValueT>
    void LoadInspectableProperty(const GenericValueT &rGenericValue, const std::filesystem::path &rBasePath,
                                 const std::unordered_map<std::string, std::shared_ptr<FastCG::Material>> &rMaterials,
                                 const std::unordered_map<std::string, std::shared_ptr<FastCG::Mesh>> &rMeshes,
                                 const std::unordered_map<std::string, FastCG::Texture *> &rTextures,
                                 FastCG::IInspectableProperty *pInspectableProperty)
    {
        switch (pInspectableProperty->GetType())
        {
        case FastCG::InspectablePropertyType::INSPECTABLE: {
            assert(rGenericValue.IsObject());
            FastCG::Inspectable *pInspectable;
            pInspectableProperty->GetValue(&pInspectable);
            LoadInspectable(rGenericValue.GetObj(), rBasePath, rMaterials, rMeshes, rTextures, pInspectable);
        }
        break;
        case FastCG::InspectablePropertyType::ENUM: {
            assert(rGenericValue.IsString());
            auto *pInspectableEnumProperty = static_cast<FastCG::IInspectableEnumProperty *>(pInspectableProperty);
            const auto *pValue = rGenericValue.GetString();
            pInspectableEnumProperty->SetSelectedItem(pValue);
        }
        break;
        case FastCG::InspectablePropertyType::BOOL: {
            auto value = rGenericValue.GetBool();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::INT32: {
            auto value = rGenericValue.GetInt();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::UINT32: {
            auto value = rGenericValue.GetUint();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::INT64: {
            auto value = rGenericValue.GetInt64();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::UINT64: {
            auto value = rGenericValue.GetUint64();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::FLOAT: {
            auto value = rGenericValue.GetFloat();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::DOUBLE: {
            auto value = rGenericValue.GetDouble();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC2: {
            assert(rGenericValue.IsArray());
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 2);
            auto value = glm::vec2{array[0].GetFloat(), array[1].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC3: {
            assert(rGenericValue.IsArray());
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 3);
            auto value = glm::vec3{array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC4: {
            assert(rGenericValue.IsArray());
            auto array = rGenericValue.GetArray();
            assert(array.Size() == 4);
            auto value = glm::vec4{array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat(), array[3].GetFloat()};
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::STRING: {
            assert(rGenericValue.IsString());
            std::string value = rGenericValue.GetString();
            pInspectableProperty->SetValue(&value);
        }
        break;
        case FastCG::InspectablePropertyType::MATERIAL: {
            assert(rGenericValue.IsString());
            std::string id = rGenericValue.GetString();
            auto it = rMaterials.find(id);
            assert(it != rMaterials.end());
            pInspectableProperty->SetValue(&it->second);
        }
        break;
        case FastCG::InspectablePropertyType::MESH: {
            assert(rGenericValue.IsString());
            std::string id = rGenericValue.GetString();
            auto it = rMeshes.find(id);
            assert(it != rMeshes.end());
            pInspectableProperty->SetValue(&it->second);
        }
        break;
        case FastCG::InspectablePropertyType::TEXTURE: {
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
    void LoadInspectable(const GenericObjectT &rInspectableObj, const std::filesystem::path &rBasePath,
                         const std::unordered_map<std::string, std::shared_ptr<FastCG::Material>> &rMaterials,
                         const std::unordered_map<std::string, std::shared_ptr<FastCG::Mesh>> &rMeshes,
                         const std::unordered_map<std::string, FastCG::Texture *> &rTextures,
                         FastCG::Inspectable *pInspectable, const std::vector<std::string> &rPropertiesToIgnore)
    {
        for (auto it = rInspectableObj.MemberBegin(); it != rInspectableObj.MemberEnd(); ++it)
        {
            std::string propertyName = it->name.GetString();
            if (std::find(rPropertiesToIgnore.begin(), rPropertiesToIgnore.end(), propertyName) !=
                rPropertiesToIgnore.end())
            {
                continue;
            }
            auto *pInspectableProperty = pInspectable->GetInspectableProperty(propertyName);
            // ignore non-existing inspectable properties
            if (pInspectableProperty == nullptr)
            {
                FASTCG_LOG_WARN(GameObjectLoader, "Ignoring non-existing inspectable property (property: %s)",
                                propertyName.c_str());
                continue;
            }
            LoadInspectableProperty(it->value, rBasePath, rMaterials, rMeshes, rTextures, pInspectableProperty);
        }
    }

    template <typename GenericObjectT>
    FastCG::GameObject *LoadGameObject(
        const GenericObjectT &rGameObjectObj, const std::filesystem::path &rBasePath,
        const std::unordered_map<std::string, std::shared_ptr<FastCG::Material>> &rMaterials,
        const std::unordered_map<std::string, std::shared_ptr<FastCG::Mesh>> &rMeshes,
        const std::unordered_map<std::string, FastCG::Texture *> &rTextures, FastCG::GameObject *pParent)
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
                position =
                    glm::vec3{positionArray[0].GetFloat(), positionArray[1].GetFloat(), positionArray[2].GetFloat()};
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
                    LoadInspectable(componentObj, rBasePath, rMaterials, rMeshes, rTextures, pComponent, {"name"});
                }
                else
                {
                    FASTCG_LOG_WARN(GameObjectLoader, "Ignoring non-existing component (component: %s)",
                                    componentName.c_str());
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
    GameObject *GameObjectLoader::Load(const std::filesystem::path &rFilePath, GameObjectoaderOptionMaskType options)
    {
        FASTCG_UNUSED(options); // TODO: add options

        size_t size;
        auto data = FileReader::ReadText(rFilePath, size);
        if (size == 0)
        {
            return nullptr;
        }
        std::string jsonStr(data.get(), data.get() + size);

        auto basePath = rFilePath.parent_path();

        rapidjson::Document document;
        document.Parse(jsonStr.c_str());

        assert(document.IsArray());
        auto docArray = document.GetArray();
        assert(docArray.Size() == 1 && docArray[0].IsObject());
        auto docObj = docArray[0].GetObj();

        std::unordered_map<std::string, Texture *> textures;
        std::unordered_map<std::string, std::shared_ptr<Material>> materials;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
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
                    materials.emplace(id, LoadMaterial(refObj["value"].GetObj(), textures));
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
                    meshes.emplace(id, LoadMesh(meshObj, basePath));
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