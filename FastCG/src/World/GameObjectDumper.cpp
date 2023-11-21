#include <FastCG/World/Transform.h>
#include <FastCG/World/GameObjectDumper.h>
#include <FastCG/World/Component.h>
#include <FastCG/Reflection/Inspectable.h>
#include <FastCG/Platform/FileWriter.h>
#include <FastCG/Platform/File.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Base64.h>

#ifdef FASTCG_LINUX
// X11 defines Bool and rapidjson uses Bool as a member-function identifier
#ifdef Bool
#undef Bool
#endif
#endif
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>

namespace
{
    template <typename GenericObjectT, typename T>
    auto CreateValue(GenericObjectT &rGenericObj, T value)
    {
        FASTCG_UNUSED(rGenericObj);
        return rapidjson::Value(value);
    }

    template <typename AllocatorT, typename GenericObjectT>
    auto CreateValue(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rValue)
    {
        FASTCG_UNUSED(rGenericObj);
        return rapidjson::Value(rValue.c_str(), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    auto CreateValue(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const glm::vec2 &rValue)
    {
        FASTCG_UNUSED(rGenericObj);
        rapidjson::Value array(rapidjson::kArrayType);
        for (int i = 0; i < 2; ++i)
        {
            array.PushBack(rValue[i], rAlloc);
        }
        return array;
    }

    template <typename AllocatorT, typename GenericObjectT>
    auto CreateValue(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const glm::vec3 &rValue)
    {
        FASTCG_UNUSED(rGenericObj);
        rapidjson::Value array(rapidjson::kArrayType);
        for (int i = 0; i < 3; ++i)
        {
            array.PushBack(rValue[i], rAlloc);
        }
        return array;
    }

    template <typename AllocatorT, typename GenericObjectT>
    auto CreateValue(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const glm::vec4 &rValue)
    {
        FASTCG_UNUSED(rGenericObj);
        rapidjson::Value array(rapidjson::kArrayType);
        for (int i = 0; i < 4; ++i)
        {
            array.PushBack(rValue[i], rAlloc);
        }
        return array;
    }

    template <typename AllocatorT, typename GenericObjectT>
    auto CreateValue(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const glm::ivec4 &rValue)
    {
        FASTCG_UNUSED(rGenericObj);
        rapidjson::Value array(rapidjson::kArrayType);
        for (int i = 0; i < 4; ++i)
        {
            array.PushBack(rValue[i], rAlloc);
        }
        return array;
    }

    template <typename AllocatorT, typename GenericObjectT>
    auto CreateValue(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const glm::quat &rValue)
    {
        FASTCG_UNUSED(rGenericObj);
        rapidjson::Value array(rapidjson::kArrayType);
        array.PushBack(rValue.w, rAlloc);
        array.PushBack(rValue.x, rAlloc);
        array.PushBack(rValue.y, rAlloc);
        array.PushBack(rValue.z, rAlloc);
        return array;
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, bool value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, int32_t value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, uint32_t value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, int64_t value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, uint64_t value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, float value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, double value)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rGenericObj, value), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const char *pMemberValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, pMemberValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const std::string &rMemberValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, rMemberValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const glm::vec2 &rValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, rValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const glm::vec3 &rValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, rValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const glm::vec4 &rValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, rValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const glm::ivec4 &rValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, rValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT>
    void AddValueMember(AllocatorT &rAlloc, GenericObjectT &rGenericObj, const std::string &rMemberName, const glm::quat &rValue)
    {
        rGenericObj.AddMember(CreateValue(rAlloc, rGenericObj, rMemberName), CreateValue(rAlloc, rGenericObj, rValue), rAlloc);
    }

    template <typename AllocatorT, typename GenericObjectT, typename GenericValueT>
    auto &AddMember(AllocatorT &rAlloc, GenericObjectT &rObj, const std::string &rMemberName, GenericValueT &rMemberObj)
    {
        return rObj.AddMember(CreateValue(rAlloc, rObj, rMemberName), rMemberObj, rAlloc);
    }

    template <typename T>
    std::string GetId(const T *pObject)
    {
        return std::to_string((uint64_t)pObject);
    }

    template <typename T>
    std::string GetId(const std::shared_ptr<T> &pObject)
    {
        return std::to_string((uint64_t)pObject.get());
    }

    template <typename AllocatorT, typename GenericObjectT>
    void DumpTexture(const FastCG::Texture *,
                     const std::string &,
                     FastCG::GameObjectDumperOptionMaskType,
                     AllocatorT &,
                     GenericObjectT &);

    template <typename AllocatorT, typename GenericObjectT>
    void DumpMaterial(const std::shared_ptr<FastCG::Material> &pMaterial,
                      const std::string &rBasePath,
                      FastCG::GameObjectDumperOptionMaskType options,
                      AllocatorT &rAlloc,
                      GenericObjectT &rMaterialObj,
                      std::unordered_map<std::string, rapidjson::Value> &rTextures)
    {
        AddValueMember(rAlloc, rMaterialObj, "name", pMaterial->GetName());
        AddValueMember(rAlloc, rMaterialObj, "materialDefinition", pMaterial->GetMaterialDefinition()->GetName());
        if (pMaterial->GetConstantBufferSize() > 0)
        {
            auto data = FastCG::EncodeBase64(pMaterial->GetConstantBufferData(), pMaterial->GetConstantBufferSize());
            AddValueMember(rAlloc, rMaterialObj, "constantBufferData", data);
        }
        if (pMaterial->GetTextureCount() > 0)
        {
            rapidjson::Value texturesArray(rapidjson::kArrayType);
            for (size_t i = 0; i < pMaterial->GetTextureCount(); ++i)
            {
                std::string name;
                const FastCG::Texture *pTexture;
                pMaterial->GetTextureAt(i, name, pTexture);
                if (pTexture != nullptr)
                {
                    auto id = GetId(pTexture);
                    auto it = rTextures.find(id);
                    if (it == rTextures.end())
                    {
                        rapidjson::Value textureObj(rapidjson::kObjectType);
                        DumpTexture(pTexture, rBasePath, options, rAlloc, textureObj);
                        rTextures[id] = textureObj;
                    }
                    rapidjson::Value refObj(rapidjson::kObjectType);
                    AddValueMember(rAlloc, refObj, "name", name);
                    AddValueMember(rAlloc, refObj, "id", id);
                    texturesArray.PushBack(refObj, rAlloc);
                }
            }
            AddMember(rAlloc, rMaterialObj, "textures", texturesArray);
        }
    }

    template <typename AllocatorT, typename GenericObjectT>
    void DumpMesh(const std::shared_ptr<FastCG::Mesh> &pMesh,
                  const std::string &rBasePath,
                  FastCG::GameObjectDumperOptionMaskType options,
                  AllocatorT &rAlloc,
                  GenericObjectT &rMeshObj)
    {
        AddValueMember(rAlloc, rMeshObj, "name", pMesh->GetName());
        if (pMesh->GetVertexBufferCount() > 0)
        {
            rapidjson::Value vertexBuffersArray(rapidjson::kArrayType);
            const auto *const *pVertexBuffers = pMesh->GetVertexBuffers();
            for (uint32_t i = 0; i < pMesh->GetVertexBufferCount(); ++i)
            {
                rapidjson::Value vertexBufferObj(rapidjson::kObjectType);
                const auto *pVertexBuffer = pVertexBuffers[i];
                AddValueMember(rAlloc, vertexBufferObj, "name", pVertexBuffer->GetName());
                AddValueMember(rAlloc, vertexBufferObj, "usage", pVertexBuffer->GetUsage());
                if ((options & (FastCG::GameObjectDumperOptionMaskType)FastCG::GameObjectDumperOption::EMBED_RESOURCE_DATA) != 0)
                {
                    AddValueMember(rAlloc, vertexBufferObj, "dataSize", pVertexBuffer->GetDataSize());
                    auto data = FastCG::EncodeBase64(pVertexBuffer->GetData(), pVertexBuffer->GetDataSize());
                    AddValueMember(rAlloc, vertexBufferObj, "data", data);
                }
                else
                {
                    auto fileName = GetId(pMesh) + ".vbuffer" + std::to_string(i);
                    auto dataPath = FastCG::File::Join({rBasePath, fileName});
                    FastCG::FileWriter::WriteBinary(dataPath, pVertexBuffer->GetData(), pVertexBuffer->GetDataSize());
                    AddValueMember(rAlloc, vertexBufferObj, "dataPath", fileName);
                }
                auto &rBindingDescriptors = pVertexBuffer->GetVertexBindingDescriptors();
                assert(!rBindingDescriptors.empty());
                rapidjson::Value bindingDescriptorsArray(rapidjson::kArrayType);
                for (const auto &rBindingDescriptor : rBindingDescriptors)
                {
                    rapidjson::Value bindingDecriptorObj(rapidjson::kObjectType);
                    AddValueMember(rAlloc, bindingDecriptorObj, "binding", rBindingDescriptor.binding);
                    AddValueMember(rAlloc, bindingDecriptorObj, "size", rBindingDescriptor.size);
                    AddValueMember(rAlloc, bindingDecriptorObj, "type", FastCG::GetVertexDataTypeString(rBindingDescriptor.type));
                    AddValueMember(rAlloc, bindingDecriptorObj, "normalized", rBindingDescriptor.normalized);
                    AddValueMember(rAlloc, bindingDecriptorObj, "stride", rBindingDescriptor.stride);
                    AddValueMember(rAlloc, bindingDecriptorObj, "offset", rBindingDescriptor.offset);
                    bindingDescriptorsArray.PushBack(bindingDecriptorObj, rAlloc);
                }
                AddMember(rAlloc, vertexBufferObj, "bindingDescriptors", bindingDescriptorsArray);
                vertexBuffersArray.PushBack(vertexBufferObj, rAlloc);
            }
            AddMember(rAlloc, rMeshObj, "vertexBuffers", vertexBuffersArray);
        }
        rapidjson::Value indexBufferObj(rapidjson::kObjectType);
        auto *pIndexBuffer = pMesh->GetIndexBuffer();
        AddValueMember(rAlloc, indexBufferObj, "usage", pIndexBuffer->GetUsage());
        if ((options & (FastCG::GameObjectDumperOptionMaskType)FastCG::GameObjectDumperOption::EMBED_RESOURCE_DATA) != 0)
        {
            AddValueMember(rAlloc, indexBufferObj, "count", pIndexBuffer->GetDataSize());
            auto data = FastCG::EncodeBase64(pIndexBuffer->GetData(), pIndexBuffer->GetDataSize());
            AddValueMember(rAlloc, indexBufferObj, "data", data);
        }
        else
        {
            auto fileName = GetId(pMesh) + ".indices";
            auto dataPath = FastCG::File::Join({rBasePath, fileName});
            FastCG::FileWriter::WriteBinary(dataPath, pIndexBuffer->GetData(), pIndexBuffer->GetDataSize());
            AddValueMember(rAlloc, indexBufferObj, "dataPath", fileName);
        }
        AddMember(rAlloc, rMeshObj, "indexBuffer", indexBufferObj);
        auto &rBounds = pMesh->GetBounds();
        if (!rBounds.IsDegenerated())
        {
            rapidjson::Value boundObj(rapidjson::kObjectType);
            AddValueMember(rAlloc, boundObj, "min", rBounds.min);
            AddValueMember(rAlloc, boundObj, "max", rBounds.max);
            AddMember(rAlloc, rMeshObj, "bounds", boundObj);
        }
    }

    template <typename AllocatorT, typename GenericObjectT>
    void DumpTexture(const FastCG::Texture *pTexture,
                     const std::string &rBasePath,
                     FastCG::GameObjectDumperOptionMaskType options,
                     AllocatorT &rAlloc,
                     GenericObjectT &rTextureObj)
    {
        AddValueMember(rAlloc, rTextureObj, "name", pTexture->GetName());
        AddValueMember(rAlloc, rTextureObj, "width", pTexture->GetWidth());
        AddValueMember(rAlloc, rTextureObj, "height", pTexture->GetHeight());
        AddValueMember(rAlloc, rTextureObj, "type", FastCG::GetTextureTypeString(pTexture->GetType()));
        AddValueMember(rAlloc, rTextureObj, "usage", pTexture->GetUsage());
        AddValueMember(rAlloc, rTextureObj, "format", FastCG::GetTextureFormatString(pTexture->GetFormat()));
        auto &rBpp = pTexture->GetBitsPerChannel();
        AddValueMember(rAlloc, rTextureObj, "bitsPerChannel", glm::ivec4{rBpp.r, rBpp.g, rBpp.b, rBpp.a});
        AddValueMember(rAlloc, rTextureObj, "dataType", FastCG::GetTextureDataTypeString(pTexture->GetDataType()));
        AddValueMember(rAlloc, rTextureObj, "filter", FastCG::GetTextureFilterString(pTexture->GetFilter()));
        AddValueMember(rAlloc, rTextureObj, "wrapMode", FastCG::GetTextureWrapModeString(pTexture->GetWrapMode()));
        if ((options & (FastCG::GameObjectDumperOptionMaskType)FastCG::GameObjectDumperOption::EMBED_RESOURCE_DATA) != 0)
        {
            auto data = FastCG::EncodeBase64(pTexture->GetData(), pTexture->GetDataSize());
            AddValueMember(rAlloc, rTextureObj, "data", data);
        }
        else
        {
            auto fileName = GetId(pTexture) + ".texture";
            auto dataPath = FastCG::File::Join({rBasePath, fileName});
            FastCG::FileWriter::WriteBinary(dataPath, pTexture->GetData(), pTexture->GetDataSize());
            AddValueMember(rAlloc, rTextureObj, "dataPath", fileName);
        }
    }

    template <typename AllocatorT, typename GenericObjectT>
    void DumpInspectable(const FastCG::Inspectable *,
                         const std::string &,
                         FastCG::GameObjectDumperOptionMaskType,
                         AllocatorT &,
                         GenericObjectT &,
                         std::unordered_map<std::string, rapidjson::Value> &,
                         std::unordered_map<std::string, rapidjson::Value> &,
                         std::unordered_map<std::string, rapidjson::Value> &);

    template <typename AllocatorT, typename GenericValueT>
    void DumpInspectableProperty(const FastCG::IInspectableProperty *pInspectableProperty,
                                 const std::string &rBasePath,
                                 FastCG::GameObjectDumperOptionMaskType options,
                                 AllocatorT &rAlloc,
                                 GenericValueT &rInspectableObj,
                                 std::unordered_map<std::string, rapidjson::Value> &rMaterials,
                                 std::unordered_map<std::string, rapidjson::Value> &rMeshes,
                                 std::unordered_map<std::string, rapidjson::Value> &rTextures)
    {
        switch (pInspectableProperty->GetType())
        {
        case FastCG::InspectablePropertyType::INSPECTABLE:
        {
            FastCG::Inspectable *pInspectable;
            pInspectableProperty->GetValue(&pInspectable);
            rapidjson::Value inspectablePropertyObj(rapidjson::kObjectType);
            DumpInspectable(pInspectable, rBasePath, options, rAlloc, inspectablePropertyObj, rMaterials, rMeshes, rTextures);
            AddMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), inspectablePropertyObj);
        }
        break;
        case FastCG::InspectablePropertyType::ENUM:
        {
            auto *pInspectableEnumProperty = static_cast<const FastCG::IInspectableEnumProperty *>(pInspectableProperty);
            AddValueMember(rAlloc, rInspectableObj, pInspectableEnumProperty->GetName(), pInspectableEnumProperty->GetSelectedItemName());
        }
        break;
        case FastCG::InspectablePropertyType::BOOL:
        {
            bool value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::INT32:
        {
            int32_t value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::UINT32:
        {
            uint32_t value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::INT64:
        {
            int64_t value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::UINT64:
        {
            uint64_t value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::FLOAT:
        {
            float value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::DOUBLE:
        {
            double value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC2:
        {
            glm::vec2 value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC3:
        {
            glm::vec3 value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::VEC4:
        {
            glm::vec4 value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::STRING:
        {
            std::string value;
            pInspectableProperty->GetValue(&value);
            AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), value);
        }
        break;
        case FastCG::InspectablePropertyType::MATERIAL:
        {
            std::shared_ptr<FastCG::Material> pMaterial;
            pInspectableProperty->GetValue(&pMaterial);
            if (pMaterial != nullptr)
            {
                auto id = GetId(pMaterial);
                auto it = rMaterials.find(id);
                if (it == rMaterials.end())
                {
                    rapidjson::Value materialObj(rapidjson::kObjectType);
                    DumpMaterial(pMaterial, rBasePath, options, rAlloc, materialObj, rTextures);
                    rMaterials[id] = materialObj;
                }
                AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), id);
            }
        }
        break;
        case FastCG::InspectablePropertyType::MESH:
        {
            std::shared_ptr<FastCG::Mesh> pMesh;
            pInspectableProperty->GetValue(&pMesh);
            if (pMesh != nullptr)
            {
                auto id = GetId(pMesh);
                auto it = rMeshes.find(id);
                if (it == rMeshes.end())
                {
                    rapidjson::Value meshObj(rapidjson::kObjectType);
                    DumpMesh(pMesh, rBasePath, options, rAlloc, meshObj);
                    rMeshes[id] = meshObj;
                }
                AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), id);
            }
        }
        break;
        case FastCG::InspectablePropertyType::TEXTURE:
        {
            const FastCG::Texture *pTexture;
            pInspectableProperty->GetValue(&pTexture);
            if (pTexture != nullptr)
            {
                auto id = GetId(pTexture);
                auto it = rTextures.find(id);
                if (it == rTextures.end())
                {
                    rapidjson::Value textureObj(rapidjson::kObjectType);
                    DumpTexture(pTexture, rBasePath, options, rAlloc, textureObj);
                    rTextures[id] = textureObj;
                }
                AddValueMember(rAlloc, rInspectableObj, pInspectableProperty->GetName(), id);
            }
        }
        break;
        default:
            assert(false);
        }
    }

    template <typename AllocatorT, typename GenericObjectT>
    void DumpInspectable(const FastCG::Inspectable *pInspectable,
                         const std::string &rBasePath,
                         FastCG::GameObjectDumperOptionMaskType options,
                         AllocatorT &rAlloc,
                         GenericObjectT &rInspectableObj,
                         std::unordered_map<std::string, rapidjson::Value> &rMaterials,
                         std::unordered_map<std::string, rapidjson::Value> &rMeshes,
                         std::unordered_map<std::string, rapidjson::Value> &rTextures)
    {
        for (size_t i = 0; i < pInspectable->GetInspectablePropertyCount(); ++i)
        {
            auto *pInspectableProperty = pInspectable->GetInspectableProperty(i);
            DumpInspectableProperty(pInspectableProperty, rBasePath, options, rAlloc, rInspectableObj, rMaterials, rMeshes, rTextures);
        }
    }

    template <typename AllocatorT, typename GenericObjectT>
    void DumpGameObject(const FastCG::GameObject *pGameObject,
                        const std::string &rBasePath,
                        FastCG::GameObjectDumperOptionMaskType options,
                        AllocatorT &rAlloc,
                        GenericObjectT &rGameObjectObj,
                        std::unordered_map<std::string, rapidjson::Value> &rMaterials,
                        std::unordered_map<std::string, rapidjson::Value> &rMeshes,
                        std::unordered_map<std::string, rapidjson::Value> &rTextures)
    {
        AddValueMember(rAlloc, rGameObjectObj, "name", pGameObject->GetName());
        auto *pTransform = pGameObject->GetTransform();
        rapidjson::Value transformObj(rapidjson::kObjectType);
        AddValueMember(rAlloc, transformObj, "scale", pTransform->GetScale());
        AddValueMember(rAlloc, transformObj, "rotation", pTransform->GetRotation());
        AddValueMember(rAlloc, transformObj, "position", pTransform->GetPosition());
        AddMember(rAlloc, rGameObjectObj, "transform", transformObj);

        auto &rComponents = pGameObject->GetComponents();
        if (!rComponents.empty())
        {
            rapidjson::Value componentsArray(rapidjson::kArrayType);
            for (const auto *pComponent : rComponents)
            {
                rapidjson::Value componentObj(rapidjson::kObjectType);
                AddValueMember(rAlloc, componentObj, "name", pComponent->GetType().GetName());
                DumpInspectable(pComponent, rBasePath, options, rAlloc, componentObj, rMaterials, rMeshes, rTextures);
                componentsArray.PushBack(componentObj, rAlloc);
            }
            AddMember(rAlloc, rGameObjectObj, "components", componentsArray);
        }

        auto &rChildren = pGameObject->GetTransform()->GetChildren();
        if (!rChildren.empty())
        {
            rapidjson::Value childrenArray(rapidjson::kArrayType);
            for (auto *pChild : rChildren)
            {
                rapidjson::Value childObj(rapidjson::kObjectType);
                DumpGameObject(pChild->GetGameObject(), rBasePath, options, rAlloc, childObj, rMaterials, rMeshes, rTextures);
                childrenArray.PushBack(childObj, rAlloc);
            }
            AddMember(rAlloc, rGameObjectObj, "children", childrenArray);
        }
    }
}

namespace FastCG
{
    void GameObjectDumper::Dump(const std::string &rFilePath, GameObject *pRootGameObject, GameObjectDumperOptionMaskType options)
    {
        rapidjson::Document document;

        auto &rAlloc = document.GetAllocator();

        rapidjson::Value docObj(rapidjson::kObjectType);

        std::unordered_map<std::string, rapidjson::Value> materials;
        std::unordered_map<std::string, rapidjson::Value> meshes;
        std::unordered_map<std::string, rapidjson::Value> textures;

        auto basePath = File::GetBasePath(rFilePath);

        rapidjson::Value rootObj(rapidjson::kObjectType);
        DumpGameObject(pRootGameObject, basePath, options, rAlloc, rootObj, materials, meshes, textures);

        rapidjson::Value resourcesObj(rapidjson::kObjectType);

        rapidjson::Value materialsArray(rapidjson::kArrayType);
        rapidjson::Value meshesArray(rapidjson::kArrayType);
        rapidjson::Value texturesArray(rapidjson::kArrayType);

        for (auto &rKvp : materials)
        {
            rapidjson::Value refObj(rapidjson::kObjectType);
            AddValueMember(rAlloc, refObj, "id", rKvp.first);
            AddMember(rAlloc, refObj, "value", rKvp.second);
            materialsArray.PushBack(refObj, rAlloc);
        }
        for (auto &rKvp : meshes)
        {
            rapidjson::Value refObj(rapidjson::kObjectType);
            AddValueMember(rAlloc, refObj, "id", rKvp.first);
            AddMember(rAlloc, refObj, "value", rKvp.second);
            meshesArray.PushBack(refObj, rAlloc);
        }
        for (auto &rKvp : textures)
        {
            rapidjson::Value refObj(rapidjson::kObjectType);
            AddValueMember(rAlloc, refObj, "id", rKvp.first);
            AddMember(rAlloc, refObj, "value", rKvp.second);
            texturesArray.PushBack(refObj, rAlloc);
        }

        AddMember(rAlloc, resourcesObj, "materials", materialsArray);
        AddMember(rAlloc, resourcesObj, "meshes", meshesArray);
        AddMember(rAlloc, resourcesObj, "textures", texturesArray);

        AddMember(rAlloc, docObj, "resources", resourcesObj);

        AddMember(rAlloc, docObj, "root", rootObj);

        document.SetArray().PushBack(docObj, rAlloc);

        rapidjson::StringBuffer stringBuffer;
        stringBuffer.Clear();
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);
        document.Accept(writer);

        std::string jsonStr = stringBuffer.GetString();

        FileWriter::WriteText(rFilePath, jsonStr.data(), jsonStr.size());
    }

}