#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/Core/Enums.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Graphics/RenderingPath.h>
#include <FastCG/Graphics/ShaderImporter.h>
#include <FastCG/Graphics/ShaderSource.h>
#include <FastCG/Platform/Application.h>
#include <FastCG/Platform/FileReader.h>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace
{
    FASTCG_DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(FastCG::ShaderType, const char *, SHADER_TYPE_TEXT_FILE_EXTENSIONS,
                                              ".vert", ".frag", ".comp");
    FASTCG_DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(FastCG::ShaderType, const char *, SHADER_TYPE_BINARY_FILE_EXTENSIONS,
                                              ".vert_spv", ".frag_spv", ".comp_spv");

    FastCG::RenderingPathMask GetSuitableRenderingPathMask(const std::filesystem::path &rShaderFilePath)
    {
        for (FastCG::RenderingPathInt i = 0; i < (FastCG::RenderingPathInt)FastCG::RenderingPath::LAST; ++i)
        {
            if (rShaderFilePath.string().find(FastCG::RENDERING_PATH_PATH_PATTERNS[i]) != std::string::npos)
            {
                return (1 << i);
            }
        }

        // if it didn't match any specific rendering path pattern, then it's suitable to all
        return (1 << (FastCG::RenderingPathInt)FastCG::RenderingPath::LAST) - 1;
    }

    bool GetShaderInfo(const std::string &rExtension, FastCG::ShaderType &rShaderType, bool &rIsText)
    {
        for (FastCG::ShaderTypeInt i = 0; i < (FastCG::ShaderTypeInt)FastCG::ShaderType::LAST; ++i)
        {
            if (rExtension == SHADER_TYPE_TEXT_FILE_EXTENSIONS[i])
            {
                rShaderType = (FastCG::ShaderType)i;
                rIsText = true;
                return true;
            }
            else if (rExtension == SHADER_TYPE_BINARY_FILE_EXTENSIONS[i])
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
            ShaderTypeValueArray<std::filesystem::path> programFilePaths;
        };

        const auto renderingPath = Application::GetInstance()->GetRenderingPath();
        const auto allowedRenderingPathMask = 1 << (RenderingPathMask)renderingPath;
        std::unordered_map<std::string, ShaderInfo> shaderInfos;
        for (const auto &rShaderFileName : AssetSystem::GetInstance()->List("shaders", true))
        {
            // only import shaders from the selected rendering path
            if ((allowedRenderingPathMask & GetSuitableRenderingPathMask(rShaderFileName)) == 0)
            {
                continue;
            }

            auto shaderName = rShaderFileName.stem().string();
            auto extension = rShaderFileName.extension().string();

            ShaderType shaderType;
            bool text;
            if (!GetShaderInfo(extension, shaderType, text))
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
                FASTCG_THROW_EXCEPTION(Exception,
                                       "Shader %s mixes both text and binary sources, which is not supported",
                                       shaderName.c_str());
            }

            rShaderInfo.programFilePaths[(ShaderTypeInt)shaderType] = rShaderFileName;
        }

        FASTCG_LOG_DEBUG(ShaderImporter, "Importing shaders (%s):", GetRenderingPathString(renderingPath));

        for (const auto &rEntry : shaderInfos)
        {
            const auto &rShaderInfo = rEntry.second;
            Shader::Args shaderArgs{};
            shaderArgs.name = rEntry.first;
            shaderArgs.text = rShaderInfo.text;
            ShaderTypeValueArray<std::unique_ptr<uint8_t[]>> programsData;
            for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::LAST; ++i)
            {
                if (rShaderInfo.programFilePaths[i].empty())
                {
                    continue;
                }

                FASTCG_LOG_DEBUG(ShaderImporter, "- %s [%s] (%s)", shaderArgs.name.c_str(), ShaderType_STRINGS[i],
                                 shaderArgs.text ? "t" : "b");

                if (shaderArgs.text)
                {
                    auto programSource = ShaderSource::ParseFile(rShaderInfo.programFilePaths[i]);
                    shaderArgs.programsData[i].dataSize = programSource.size() + 1;
                    programsData[i] = std::make_unique<uint8_t[]>(shaderArgs.programsData[i].dataSize);
                    std::copy(programSource.cbegin(), programSource.cend(), (char *)programsData[i].get());
                    programsData[i][shaderArgs.programsData[i].dataSize - 1] = '\0';
                }
                else
                {
                    programsData[i] =
                        FileReader::ReadBinary(rShaderInfo.programFilePaths[i], shaderArgs.programsData[i].dataSize);
                }
                shaderArgs.programsData[i].pData = (void *)programsData[i].get();
            }
            GraphicsSystem::GetInstance()->CreateShader(shaderArgs);
        }
    }
}
