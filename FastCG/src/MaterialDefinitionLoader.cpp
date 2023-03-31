#include <FastCG/TextureLoader.h>
#include <FastCG/MaterialDefinitionLoader.h>
#include <FastCG/Graphics/GraphicsContextState.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>
#include <FastCG/ConstantBuffer.h>
#include <FastCG/AssetSystem.h>

#ifdef FASTCG_LINUX
// X11 defines Bool and rapidjson uses Bool as a member-function identifier
#ifdef Bool
#undef Bool
#endif
#endif
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <glm/glm.hpp>

#include <initializer_list>
#include <algorithm>

namespace
{
    template <typename GenericObjectT>
    void GetBoolMember(const GenericObjectT &rGenericObj, const char *pName, bool &rValue)
    {
        if (rGenericObj.HasMember(pName))
        {
            auto &rGenericVal = rGenericObj.FindMember(pName)->value;
            assert(rGenericVal.IsFalse() || rGenericVal.IsTrue());
            rValue = rGenericVal.GetBool();
        }
    }

    template <typename GenericValueT>
    void GetInt32Value(const GenericValueT &rGenericVal, int32_t &rValue)
    {
        assert(rGenericVal.IsInt());
        rValue = rGenericVal.GetInt();
    }

    template <typename GenericObjectT>
    void GetInt32Member(const GenericObjectT &rGenericObj, const char *pName, int32_t &rValue)
    {
        if (rGenericObj.HasMember(pName))
        {
            auto &rGenericVal = rGenericObj.FindMember(pName)->value;
            GetInt32Value(rGenericVal, rValue);
        }
    }

    template <typename GenericValueT>
    void GetUint32Value(const GenericValueT &rGenericVal, uint32_t &rValue)
    {
        assert(rGenericVal.IsUint());
        rValue = rGenericVal.GetUint();
    }

    template <typename GenericObjectT>
    void GetUint32Member(const GenericObjectT &rGenericObj, const char *pName, uint32_t &rValue)
    {
        if (rGenericObj.HasMember(pName))
        {
            auto &rGenericVal = rGenericObj.FindMember(pName)->value;
            GetUint32Value(rGenericVal, rValue);
        }
    }

    template <typename GenericValueT, typename EnumT>
    void GetEnumValue(const GenericValueT &rGenericValue, EnumT &rValue, const char *const *pValues, size_t valueCount)
    {
        static_assert(std::is_enum<EnumT>::value, "EnumT must be an enumerator");
        assert(rGenericValue.IsString());
        std::string valueStr = rGenericValue.GetString();
        auto pValuesEnd = pValues + valueCount;
        auto it = std::find(pValues, pValuesEnd, valueStr);
        assert(it != pValuesEnd);
        rValue = (EnumT)std::distance(pValues, it);
    }

    template <typename GenericObjectT, typename EnumT>
    void GetEnumMember(const GenericObjectT &rGenericObj, const char *pName, EnumT &rValue, const char *const *rValues, size_t valueCount)
    {
        static_assert(std::is_enum<EnumT>::value, "EnumT must be an enumerator");
        if (rGenericObj.HasMember(pName))
        {
            GetEnumValue(rGenericObj.FindMember(pName)->value, rValue, rValues, valueCount);
        }
    }

}

namespace FastCG
{
    const MaterialDefinition *MaterialDefinitionLoader::Load(const std::string &rFilePath)
    {
        auto basePath = File::GetBasePath(rFilePath);

        size_t size;
        auto data = FileReader::ReadText(rFilePath, size);
        std::string jsonStr(data.get(), data.get() + size);

        rapidjson::Document document;
        document.Parse(jsonStr.c_str());

        assert(document.HasMember("shader") && document["shader"].IsString());
        auto *pShader = GraphicsSystem::GetInstance()->FindShader(document["shader"].GetString());
        assert(pShader != nullptr);

        std::vector<ConstantBuffer::Member> constantBufferMembers;
        if (document.HasMember("constantBuffer"))
        {
            assert(document["constantBuffer"].IsArray());
            auto membersArray = document["constantBuffer"].GetArray();
            constantBufferMembers.reserve(membersArray.Size());
            for (auto &rMemberEl : membersArray)
            {
                assert(rMemberEl.IsArray());
                auto memberArray = rMemberEl.GetArray();
                assert(memberArray.Size() == 2);
                assert(memberArray[0].IsString());
                if (memberArray[1].IsNumber())
                {
                    constantBufferMembers.emplace_back(memberArray[0].GetString(), memberArray[1].GetFloat());
                }
                else
                {
                    assert(memberArray[1].IsArray());
                    auto valueArray = memberArray[1].GetArray();
                    if (valueArray.Size() == 2)
                    {
                        constantBufferMembers.emplace_back(memberArray[0].GetString(), glm::vec2{valueArray[0].GetFloat(), valueArray[1].GetFloat()});
                    }
                    else
                    {
                        assert(valueArray.Size() == 4);
                        constantBufferMembers.emplace_back(memberArray[0].GetString(), glm::vec4{valueArray[0].GetFloat(), valueArray[1].GetFloat(), valueArray[2].GetFloat(), valueArray[3].GetFloat()});
                    }
                }
            }
        }

        std::unordered_map<std::string, const Texture *> textures;
        if (document.HasMember("textures"))
        {
            assert(document["textures"].IsArray());
            for (auto &rTextureEl : document["textures"].GetArray())
            {
                assert(rTextureEl.IsArray());
                auto textureArray = rTextureEl.GetArray();
                assert(textureArray.Size() == 2);
                assert(textureArray[0].IsString());
                if (textureArray[1].IsString())
                {
                    textures.emplace(textureArray[0].GetString(), TextureLoader::Load(File::Join({basePath, textureArray[1].GetString()})));
                }
                else
                {
                    assert(textureArray[1].IsNull());
                    textures.emplace(textureArray[0].GetString(), nullptr);
                }
            }
        }

        GraphicsContextState graphicsContextState;
        if (document.HasMember("graphicsContextState"))
        {
            assert(document["graphicsContextState"].IsObject());
            auto graphicsContextStateObj = document["graphicsContextState"].GetObj();

            GetBoolMember(graphicsContextStateObj, "depthTest", graphicsContextState.depthTest);
            GetBoolMember(graphicsContextStateObj, "depthWrite", graphicsContextState.depthWrite);
            GetEnumMember(graphicsContextStateObj, "depthFunc", graphicsContextState.depthFunc, COMPARE_OP_STRINGS, FASTCG_ARRAYSIZE(COMPARE_OP_STRINGS));
            GetBoolMember(graphicsContextStateObj, "scissorTest", graphicsContextState.scissorTest);
            GetBoolMember(graphicsContextStateObj, "stencilTest", graphicsContextState.stencilTest);
            struct Iter
            {
                const char *pMemberName;
                StencilState &rStencilState;
            };
            for (auto &rIter : {Iter{"stencilBackState", graphicsContextState.stencilBackState}, Iter{"stencilFrontState", graphicsContextState.stencilFrontState}})
            {
                if (graphicsContextStateObj.HasMember(rIter.pMemberName))
                {
                    assert(graphicsContextStateObj.FindMember(rIter.pMemberName)->value.IsObject());
                    auto stencilStateObj = graphicsContextStateObj.FindMember(rIter.pMemberName)->value.GetObj();
                    GetEnumMember(stencilStateObj, "compareOp", rIter.rStencilState.compareOp, COMPARE_OP_STRINGS, FASTCG_ARRAYSIZE(COMPARE_OP_STRINGS));
                    GetEnumMember(stencilStateObj, "passOp", rIter.rStencilState.passOp, STENCIL_OP_STRINGS, FASTCG_ARRAYSIZE(STENCIL_OP_STRINGS));
                    GetEnumMember(stencilStateObj, "stencilFailOp", rIter.rStencilState.stencilFailOp, STENCIL_OP_STRINGS, FASTCG_ARRAYSIZE(STENCIL_OP_STRINGS));
                    GetEnumMember(stencilStateObj, "depthFailOp", rIter.rStencilState.depthFailOp, STENCIL_OP_STRINGS, FASTCG_ARRAYSIZE(STENCIL_OP_STRINGS));
                    GetInt32Member(stencilStateObj, "reference", rIter.rStencilState.reference);
                    GetUint32Member(stencilStateObj, "compareMask", rIter.rStencilState.compareMask);
                    GetUint32Member(stencilStateObj, "writeMask", rIter.rStencilState.writeMask);
                }
            }
            GetEnumMember(graphicsContextStateObj, "cullMode", graphicsContextState.cullMode, FACE_STRINGS, FASTCG_ARRAYSIZE(FACE_STRINGS));
            GetBoolMember(graphicsContextStateObj, "blend", graphicsContextState.blend);
            if (graphicsContextStateObj.HasMember("blendState"))
            {
                assert(graphicsContextStateObj.FindMember("blendState")->value.IsObject());
                auto blendStateObj = graphicsContextStateObj.FindMember("blendState")->value.GetObj();
                GetEnumMember(blendStateObj, "alphaOp", graphicsContextState.blendState.alphaOp, BLEND_FUNC_STRINGS, FASTCG_ARRAYSIZE(BLEND_FUNC_STRINGS));
                GetEnumMember(blendStateObj, "srcAlphaFactor", graphicsContextState.blendState.srcAlphaFactor, BLEND_FACTOR_STRINGS, FASTCG_ARRAYSIZE(BLEND_FACTOR_STRINGS));
                GetEnumMember(blendStateObj, "dstAlphaFactor", graphicsContextState.blendState.dstAlphaFactor, BLEND_FACTOR_STRINGS, FASTCG_ARRAYSIZE(BLEND_FACTOR_STRINGS));
                GetEnumMember(blendStateObj, "colorOp", graphicsContextState.blendState.colorOp, BLEND_FUNC_STRINGS, FASTCG_ARRAYSIZE(BLEND_FUNC_STRINGS));
                GetEnumMember(blendStateObj, "srcColorFactor", graphicsContextState.blendState.srcColorFactor, BLEND_FACTOR_STRINGS, FASTCG_ARRAYSIZE(BLEND_FACTOR_STRINGS));
                GetEnumMember(blendStateObj, "dstColorFactor", graphicsContextState.blendState.dstColorFactor, BLEND_FACTOR_STRINGS, FASTCG_ARRAYSIZE(BLEND_FACTOR_STRINGS));
            }
        }

        return GraphicsSystem::GetInstance()->CreateMaterialDefinition({File::GetFileNameWithoutExtension(rFilePath), pShader, {constantBufferMembers}, textures, graphicsContextState});
    }

    std::string MaterialDefinitionLoader::Dump(const MaterialDefinition *pMaterialDefinition)
    {
        rapidjson::Document document;

        auto &rAlloc = document.GetAllocator();

        {
            rapidjson::Value shaderEl(rapidjson::kStringType);
            const auto &rShaderName = pMaterialDefinition->GetShader()->GetName();
            shaderEl.SetString(rShaderName.c_str(), (rapidjson::SizeType)rShaderName.size(), rAlloc);
            document.AddMember("shader", shaderEl, rAlloc);
        }

        // TODO:

        rapidjson::StringBuffer stringBuffer;
        stringBuffer.Clear();

        rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
        document.Accept(writer);

        return stringBuffer.GetString();
    }

}
