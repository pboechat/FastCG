#include <FastCG/StringUtils.h>
#include <FastCG/File.h>
#include <FastCG/AssetSystem.h>

#include <unordered_map>

namespace
{
    bool ExtractShaderInfo(const std::string &rShaderFileName, std::string &rShaderName, FastCG::ShaderType &rShaderType, FastCG::RenderingPathMask &shaderRenderingPathMask)
    {
        rShaderName = FastCG::File::GetFileNameWithoutExtension(rShaderFileName);

        shaderRenderingPathMask = 0;
        for (FastCG::RenderingPathInt i = 0; i < (FastCG::RenderingPathInt)FastCG::RenderingPath::MAX; i++)
        {
            if (rShaderFileName.find(FastCG::RENDERING_PATH_PATH_PATTERNS[i]) != std::string::npos)
            {
                shaderRenderingPathMask = (1 << i);
                break;
            }
        }

        if (shaderRenderingPathMask == 0)
        {
            shaderRenderingPathMask = (1 << (FastCG::RenderingPathInt)FastCG::RenderingPath::MAX) - 1;
        }

        if (rShaderFileName.find(FastCG::ShaderFileExtension<FastCG::ShaderType::VERTEX>::value()) != std::string::npos)
        {
            rShaderType = FastCG::ShaderType::VERTEX;
            return true;
        }
        else if (rShaderFileName.find(FastCG::ShaderFileExtension<FastCG::ShaderType::FRAGMENT>::value()) != std::string::npos)
        {
            rShaderType = FastCG::ShaderType::FRAGMENT;
            return true;
        }
        else
        {
            return false;
        }
    }

}

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
}