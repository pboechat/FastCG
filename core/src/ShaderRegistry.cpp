#ifdef USE_PROGRAMMABLE_PIPELINE

#include <ShaderRegistry.h>
#include <Directory.h>
#include <StringUtils.h>
#include <Exception.h>

#include <vector>

std::map<std::string, ShaderPtr> ShaderRegistry::mShadersByName;

bool ShaderRegistry::ExtractShaderInfo(const std::string& rShaderFileName, std::string& rShaderName, ShaderType& rShaderType)
{
	rShaderName = rShaderFileName;
	if (rShaderFileName.find(".vert") != std::string::npos)
	{
		rShaderType = ST_VERTEX;
		StringUtils::Replace(rShaderName, ".vert", "");
		return true;
	} 
	else if (rShaderFileName.find(".frag") != std::string::npos)
	{
		rShaderType = ST_FRAGMENT;
		StringUtils::Replace(rShaderName, ".frag", "");
		return true;
	}
	else
	{
		return false;
	}
}

void ShaderRegistry::LoadShadersFromDisk(const std::string& rShadersDirectory)
{
	std::vector<std::string> shaderFiles = Directory::ListFiles(rShadersDirectory);

	for (unsigned int i = 0; i < shaderFiles.size(); i++)
	{
		std::string shaderName;
		ShaderType shaderType;

		if (!ExtractShaderInfo(shaderFiles[i], shaderName, shaderType))
		{
			continue;
		}

		std::map<std::string, ShaderPtr>::iterator it = mShadersByName.find(shaderName);
		ShaderPtr shaderPtr;
		if (it == mShadersByName.end())
		{
			shaderPtr = new Shader(shaderName);
			mShadersByName.insert(std::make_pair(shaderName, shaderPtr));
		}
		else 
		{
			shaderPtr = it->second;
		}

		shaderPtr->Compile(rShadersDirectory + "/" + shaderFiles[i], shaderType);
	}

	std::map<std::string, ShaderPtr>::iterator it = mShadersByName.begin();
	while (it != mShadersByName.end())
	{
		it->second->Link();
		it++;
	}
}

ShaderPtr ShaderRegistry::Find(const std::string& rShaderName)
{
	std::map<std::string, ShaderPtr>::iterator it = mShadersByName.find(rShaderName);

	if (it == mShadersByName.end())
	{
		THROW_EXCEPTION(Exception, "Shader not found: %s", rShaderName.c_str());
	}

	return it->second;
}

#endif