#include <FastCG/StringUtils.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/Directory.h>
#include <FastCG/Exception.h>

#include <vector>

namespace FastCG
{
	const std::string VERTEX_SHADER_FILE_EXTENSION = ".vert";
	const std::string FRAGMENT_SHADER_FILE_EXTENSION = ".frag";

	std::map<std::string, std::shared_ptr<Shader>> ShaderRegistry::mShadersByName;

	bool ShaderRegistry::ExtractShaderInfo(const std::string &rShaderFileName, std::string &rShaderName, ShaderType &rShaderType)
	{
		rShaderName = rShaderFileName;
		if (rShaderFileName.find(VERTEX_SHADER_FILE_EXTENSION) != std::string::npos)
		{
			rShaderType = ShaderType::ST_VERTEX;
			StringUtils::Replace(rShaderName, VERTEX_SHADER_FILE_EXTENSION, "");
			return true;
		}
		else if (rShaderFileName.find(FRAGMENT_SHADER_FILE_EXTENSION) != std::string::npos)
		{
			rShaderType = ShaderType::ST_FRAGMENT;
			StringUtils::Replace(rShaderName, FRAGMENT_SHADER_FILE_EXTENSION, "");
			return true;
		}
		else
		{
			return false;
		}
	}

	void ShaderRegistry::LoadShadersFromDisk(const std::string &rShadersDirectory)
	{
		for (auto &rShaderFile : Directory::ListFiles(rShadersDirectory))
		{
			std::string shaderName;
			ShaderType shaderType;
			if (!ExtractShaderInfo(rShaderFile, shaderName, shaderType))
			{
				continue;
			}

			std::shared_ptr<Shader> pShader;
			auto it = mShadersByName.find(shaderName);
			if (it == mShadersByName.end())
			{
				pShader = std::make_shared<Shader>(shaderName);
				mShadersByName.insert({shaderName, pShader});
			}
			else
			{
				pShader = it->second;
			}
			pShader->Compile(rShadersDirectory + "/" + rShaderFile, shaderType);
		}

		auto it = mShadersByName.begin();
		while (it != mShadersByName.end())
		{
			it->second->Link();
			it++;
		}
	}

	std::shared_ptr<Shader> ShaderRegistry::Find(const std::string &rShaderName)
	{
		auto it = mShadersByName.find(rShaderName);
		if (it == mShadersByName.end())
		{
			FASTCG_THROW_EXCEPTION(Exception, "Shader not found: %s", rShaderName.c_str());
		}

		return it->second;
	}

	void ShaderRegistry::Unload()
	{
		mShadersByName.clear();
	}

}