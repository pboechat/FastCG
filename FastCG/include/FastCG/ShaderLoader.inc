#include <FastCG/StringUtils.h>
#include <FastCG/File.h>
#include <FastCG/AssetSystem.h>

#include <unordered_map>

namespace FastCG
{
    void ShaderLoader::LoadShaders(RenderingPath renderingPath)
    {
        const auto renderingPathMask = 1 << (RenderingPathMask)renderingPath;
        std::unordered_map<std::string, ShaderTypeValueArray<std::string>> shaderInfos;
        for (const auto &rShaderFileName : AssetSystem::GetInstance()->List("shaders", true))
        {
            std::string shaderName;
            ShaderType shaderType;
            RenderingPathMask shaderRenderingPathMask;
            if (ExtractShaderInfo(rShaderFileName, shaderName, shaderType, shaderRenderingPathMask) && (renderingPathMask & shaderRenderingPathMask) != 0)
            {
                shaderInfos[shaderName][(ShaderTypeInt)shaderType] = rShaderFileName;
            }
        }
        for (const auto &rShaderInfo : shaderInfos)
        {
            RenderingSystem::GetInstance()->CreateShader({rShaderInfo.first, rShaderInfo.second});
        }
    }

    bool ShaderLoader::ExtractShaderInfo(const std::string &rShaderFileName, std::string &rShaderName, ShaderType &rShaderType, RenderingPathMask &shaderRenderingPathMask)
    {
        rShaderName = File::GetFileNameWithoutExtension(rShaderFileName);

        shaderRenderingPathMask = 0;
        for (RenderingPathInt i = 0; i < (RenderingPathInt)RenderingPath::MAX; i++)
        {
            if (rShaderFileName.find(RENDERING_PATH_PATH_PATTERNS[i]) != std::string::npos)
            {
                shaderRenderingPathMask = (1 << i);
                break;
            }
        }

        if (shaderRenderingPathMask == 0)
        {
            shaderRenderingPathMask = (1 << (RenderingPathInt)RenderingPath::MAX) - 1;
        }

        if (rShaderFileName.find(ShaderFileExtension<ShaderType::ST_VERTEX>::value()) != std::string::npos)
        {
            rShaderType = ShaderType::ST_VERTEX;
            return true;
        }
        else if (rShaderFileName.find(ShaderFileExtension<ShaderType::ST_FRAGMENT>::value()) != std::string::npos)
        {
            rShaderType = ShaderType::ST_FRAGMENT;
            return true;
        }
        else
        {
            return false;
        }
    }
}