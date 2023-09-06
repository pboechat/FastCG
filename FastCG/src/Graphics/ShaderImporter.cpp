#include <FastCG/Graphics/ShaderSource.h>
#include <FastCG/Graphics/ShaderImporter.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Platform/FileReader.h>
#include <FastCG/Platform/File.h>
#include <FastCG/Platform/Application.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Assets/AssetSystem.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>

namespace
{
#define DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(enum, arrayType, array, ...) \
    constexpr arrayType array[] = {__VA_ARGS__};                        \
    static_assert((size_t) enum ::LAST == FASTCG_ARRAYSIZE(array),      \
                  "Missing element in " #array)

    DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(FastCG::ShaderType, const char *, SHADER_TYPE_TEXT_FILE_EXTENSIONS, ".vert", ".frag");
    DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(FastCG::ShaderType, const char *, SHADER_TYPE_BINARY_FILE_EXTENSIONS, ".vert_spv", ".frag_spv");

    FastCG::RenderingPathMask GetSuitableRenderingPathMask(const std::string &rFileName)
    {
        for (FastCG::RenderingPathInt i = 0; i < (FastCG::RenderingPathInt)FastCG::RenderingPath::LAST; ++i)
        {
            if (rFileName.find(FastCG::RENDERING_PATH_PATH_PATTERNS[i]) != std::string::npos)
            {
                return (1 << i);
            }
        }

        // if it didn't match any specific rendering path pattern, then it's suitable to all
        return (1 << (FastCG::RenderingPathInt)FastCG::RenderingPath::LAST) - 1;
    }

    bool GetShaderInfo(const std::string &rExt, FastCG::ShaderType &rShaderType, bool &rIsText)
    {
        for (FastCG::ShaderTypeInt i = 0; i < (FastCG::ShaderTypeInt)FastCG::ShaderType::LAST; ++i)
        {
            if (rExt == SHADER_TYPE_TEXT_FILE_EXTENSIONS[i])
            {
                rShaderType = (FastCG::ShaderType)i;
                rIsText = true;
                return true;
            }
            else if (rExt == SHADER_TYPE_BINARY_FILE_EXTENSIONS[i])
            {
                rShaderType = (FastCG::ShaderType)i;
                rIsText = false;
                return true;
            }
        }

        return false;
    }

}

namespace FastCG
{
    void ShaderImporter::Import()
    {
        struct ShaderInfo
        {
            bool text;
            ShaderTypeValueArray<std::string> programFileNames;
        };

        const auto allowedRenderingPathMask = 1 << (RenderingPathMask)Application::GetInstance()->GetRenderingPath();
        std::unordered_map<std::string, ShaderInfo> shaderInfos;
        for (const auto &rShaderFileName : AssetSystem::GetInstance()->List("shaders", true))
        {
            // only import shaders fro mthe selected rendering path
            if ((allowedRenderingPathMask & GetSuitableRenderingPathMask(rShaderFileName)) == 0)
            {
                continue;
            }

            std::string shaderName;
            std::string shaderExt;
            FastCG::File::SplitExt(rShaderFileName, shaderName, shaderExt);

            ShaderType shaderType;
            bool text;
            if (!GetShaderInfo(shaderExt, shaderType, text))
            {
                continue;
            }

            auto it = shaderInfos.find(shaderName);
            if (it == shaderInfos.end())
            {
                it = shaderInfos.emplace(shaderName, ShaderInfo{text}).first;
            }

            auto &rShaderInfo = it->second;
            if (rShaderInfo.text != text)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Shader %s mixes both text and binary sources, which is not supported", shaderName.c_str());
            }

            rShaderInfo.programFileNames[(ShaderTypeInt)shaderType] = rShaderFileName;
        }
        Shader::Args shaderArgs{};
        for (const auto &rEntry : shaderInfos)
        {
            const auto &rShaderInfo = rEntry.second;
            shaderArgs.name = rEntry.first;
            shaderArgs.text = rShaderInfo.text;
            ShaderTypeValueArray<std::unique_ptr<uint8_t[]>> programsData;
            for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::LAST; ++i)
            {
                if (rShaderInfo.programFileNames.empty())
                {
                    continue;
                }

                if (shaderArgs.text)
                {
                    auto programSource = ShaderSource::ParseFile(rShaderInfo.programFileNames[i]);
                    shaderArgs.programsData[i].dataSize = programSource.size() + 1;
                    programsData[i] = std::make_unique<uint8_t[]>(shaderArgs.programsData[i].dataSize);
                    std::copy(programSource.cbegin(), programSource.cend(), (char *)programsData[i].get());
                    programsData[i][shaderArgs.programsData[i].dataSize - 1] = '\0';
                }
                else
                {
                    programsData[i] = FileReader::ReadBinary(rShaderInfo.programFileNames[i], shaderArgs.programsData[i].dataSize);
                }
                shaderArgs.programsData[i].pData = (void *)programsData[i].get();
            }
            GraphicsSystem::GetInstance()->CreateShader(shaderArgs);
        }
    }
}
