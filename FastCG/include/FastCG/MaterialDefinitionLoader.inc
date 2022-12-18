#include <FastCG/TextureImporter.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/RenderingState.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>
#include <FastCG/ConstantBuffer.h>
#include <FastCG/AssetSystem.h>

#include <rapidjson/document.h>
#include <glm/glm.hpp>

#include <initializer_list>
#include <algorithm>

namespace
{
    template <typename ObjectT>
    void GetBoolMember(const ObjectT &rGenericObj, const char *pName, bool &rValue)
    {
        if (rGenericObj.HasMember(pName))
        {
            auto &rGenericVal = rGenericObj.FindMember(pName)->value;
            assert(rGenericVal.GetType() == rapidjson::Type::kFalseType || rGenericVal.GetType() == rapidjson::Type::kTrueType);
            rValue = rGenericVal.GetBool();
        }
    }

    template <typename ValueT>
    void GetInt32Value(const ValueT &rGenericVal, int32_t &rValue)
    {
        assert(rGenericVal.GetType() == rapidjson::Type::kNumberType);
        rValue = rGenericVal.GetInt();
    }

    template <typename ObjectT>
    void GetInt32Member(const ObjectT &rGenericObj, const char *pName, int32_t &rValue)
    {
        if (rGenericObj.HasMember(pName))
        {
            auto &rGenericVal = rGenericObj.FindMember(pName)->value;
            GetInt32Value(rGenericVal, rValue);
        }
    }

    template <typename ValueT>
    void GetUint32Value(const ValueT &rGenericVal, uint32_t &rValue)
    {
        assert(rGenericVal.GetType() == rapidjson::Type::kNumberType);
        rValue = rGenericVal.GetUint();
    }

    template <typename ObjectT>
    void GetUint32Member(const ObjectT &rGenericObj, const char *pName, uint32_t &rValue)
    {
        if (rGenericObj.HasMember(pName))
        {
            auto &rGenericVal = rGenericObj.FindMember(pName)->value;
            GetUint32Value(rGenericVal, rValue);
        }
    }

    template <typename ValueT, typename EnumT>
    void GetEnumValue(const ValueT &rGenericValue, EnumT &rValue, const std::initializer_list<std::string> &rValueStrs)
    {
        assert(rGenericValue.GetType() == rapidjson::Type::kStringType);
        std::string valueStr = rGenericValue.GetString();
        auto it = std::find(rValueStrs.begin(), rValueStrs.end(), valueStr);
        assert(it != rValueStrs.end());
        rValue = (EnumT)std::distance(rValueStrs.begin(), it);
    }

    template <typename ObjectT, typename EnumT>
    void GetEnumMember(const ObjectT &rGenericObj, const char *pName, EnumT &rValue, const std::initializer_list<std::string> &rValueStrs)
    {
        if (rGenericObj.HasMember(pName))
        {
            GetEnumValue(rGenericObj.FindMember(pName)->value, rValue, rValueStrs);
        }
    }

}

namespace FastCG
{
    void MaterialDefinitionLoader::LoadMaterialDefinitions()
    {
        for (const auto &rMaterialFileName : AssetSystem::GetInstance()->List("materials", true))
        {
            size_t size;
            auto data = FileReader::ReadText(rMaterialFileName, size);
            std::string jsonStr(data.get(), data.get() + size);

            rapidjson::Document document;
            document.Parse(jsonStr.c_str());

            assert(document.HasMember("shader") && document["shader"].GetType() == rapidjson::Type::kStringType);
            auto *pShader = RenderingSystem::GetInstance()->FindShader(document["shader"].GetString());

            std::vector<ConstantBuffer::Member> constantBufferMembers;
            if (document.HasMember("constantBuffer"))
            {
                assert(document["constantBuffer"].GetType() == rapidjson::Type::kArrayType);
                auto membersArray = document["constantBuffer"].GetArray();
                constantBufferMembers.reserve(membersArray.Size());
                for (auto &rMemberEl : membersArray)
                {
                    assert(rMemberEl.GetType() == rapidjson::Type::kArrayType);
                    auto memberArray = rMemberEl.GetArray();
                    assert(memberArray.Size() == 2);
                    assert(memberArray[0].GetType() == rapidjson::Type::kStringType);
                    if (memberArray[1].GetType() == rapidjson::Type::kNumberType)
                    {
                        constantBufferMembers.emplace_back(memberArray[0].GetString(), memberArray[1].GetFloat());
                    }
                    else
                    {
                        assert(memberArray[1].GetType() == rapidjson::Type::kArrayType);
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
                assert(document["textures"].GetType() == rapidjson::Type::kArrayType);
                for (auto &rTextureEl : document["textures"].GetArray())
                {
                    assert(rTextureEl.GetType() == rapidjson::Type::kArrayType);
                    auto textureArray = rTextureEl.GetArray();
                    assert(textureArray.Size() == 2);
                    assert(textureArray[0].GetType() == rapidjson::Type::kStringType);
                    if (textureArray[1].GetType() == rapidjson::Type::kStringType)
                    {
                        textures.emplace(textureArray[0].GetString(), TextureImporter::Import(textureArray[1].GetString()));
                    }
                    else
                    {
                        assert(textureArray[1].GetType() == rapidjson::Type::kNullType);
                        textures.emplace(textureArray[0].GetString(), nullptr);
                    }
                }
            }

            RenderingState renderingState;
            if (document.HasMember("renderingState"))
            {
                assert(document["renderingState"].GetType() == rapidjson::Type::kObjectType);
                auto renderingStateObj = document["renderingState"].GetObj();

                GetBoolMember(renderingStateObj, "depthTest", renderingState.depthTest);
                GetBoolMember(renderingStateObj, "depthWrite", renderingState.depthWrite);
                GetEnumMember(renderingStateObj, "depthFunc", renderingState.depthFunc, {"NEVER", "LESS", "LEQUAL", "GREATER", "GEQUAL", "EQUAL", "NOT_EQU", "ALWAYS"});
                GetBoolMember(renderingStateObj, "scissorTest", renderingState.scissorTest);
                GetBoolMember(renderingStateObj, "stencilTest", renderingState.stencilTest);
                struct Iter
                {
                    const char *pMemberName;
                    StencilState &rStencilState;
                };
                for (auto &rIter : {Iter{"stencilBackState", renderingState.stencilBackState}, Iter{"stencilFrontState", renderingState.stencilFrontState}})
                {
                    if (renderingStateObj.HasMember(rIter.pMemberName))
                    {
                        assert(renderingStateObj.FindMember(rIter.pMemberName)->value.GetType() == rapidjson::Type::kObjectType);
                        auto stencilStateObj = renderingStateObj.FindMember(rIter.pMemberName)->value.GetObj();
                        GetEnumMember(stencilStateObj, "compareOp", rIter.rStencilState.compareOp, {"NEVER", "LESS", "LEQUAL", "GREATER", "GEQUAL", "EQUAL", "NOT_EQU", "ALWAYS"});
                        GetEnumMember(stencilStateObj, "passOp", rIter.rStencilState.passOp, {});
                        GetEnumMember(stencilStateObj, "stencilFailOp", rIter.rStencilState.stencilFailOp, {});
                        GetEnumMember(stencilStateObj, "depthFailOp", rIter.rStencilState.depthFailOp, {});
                        GetInt32Member(stencilStateObj, "reference", rIter.rStencilState.reference);
                        GetUint32Member(stencilStateObj, "compareMask", rIter.rStencilState.compareMask);
                        GetUint32Member(stencilStateObj, "writeMask", rIter.rStencilState.writeMask);
                    }
                }
                GetEnumMember(renderingStateObj, "cullMode", renderingState.cullMode, {"NONE", "FRONT", "BACK", "FRONT_AND_BACK"});
                GetBoolMember(renderingStateObj, "blend", renderingState.blend);
                if (renderingStateObj.HasMember("blendState"))
                {
                    assert(renderingStateObj.FindMember("blendState")->value.GetType() == rapidjson::Type::kObjectType);
                    auto blendStateObj = renderingStateObj.FindMember("blendState")->value.GetObj();
                    GetEnumMember(blendStateObj, "alphaOp", renderingState.blendState.alphaOp, {"NONE", "ADD"});
                    GetEnumMember(blendStateObj, "srcAlphaFactor", renderingState.blendState.srcAlphaFactor, {"ZERO", "ONE", "SRC_COLOR", "DST_COLOR", "SRC_ALPHA", "DST_ALPHA", "ONE_MINUS_SRC_COLOR", "ONE_MINUS_SRC_ALPHA"});
                    GetEnumMember(blendStateObj, "dstAlphaFactor", renderingState.blendState.dstAlphaFactor, {"ZERO", "ONE", "SRC_COLOR", "DST_COLOR", "SRC_ALPHA", "DST_ALPHA", "ONE_MINUS_SRC_COLOR", "ONE_MINUS_SRC_ALPHA"});
                    GetEnumMember(blendStateObj, "colorOp", renderingState.blendState.colorOp, {"NONE", "ADD"});
                    GetEnumMember(blendStateObj, "srcColorFactor", renderingState.blendState.srcColorFactor, {"ZERO", "ONE", "SRC_COLOR", "DST_COLOR", "SRC_ALPHA", "DST_ALPHA", "ONE_MINUS_SRC_COLOR", "ONE_MINUS_SRC_ALPHA"});
                    GetEnumMember(blendStateObj, "dstColorFactor", renderingState.blendState.dstColorFactor, {"ZERO", "ONE", "SRC_COLOR", "DST_COLOR", "SRC_ALPHA", "DST_ALPHA", "ONE_MINUS_SRC_COLOR", "ONE_MINUS_SRC_ALPHA"});
                }
            }

            RenderingSystem::GetInstance()->CreateMaterialDefinition({File::GetFileNameWithoutExtension(rMaterialFileName), pShader, {constantBufferMembers}, textures, renderingState});
        }
    }

}