#ifndef FIXED_FUNCTION_PIPELINE

#include <ShaderRegistry.h>
#include <Directory.h>
#include <StringUtils.h>
#include <Exception.h>

#include <vector>

const std::string ShaderRegistry::VERTEX_SHADER_FILE_EXTENSION = ".vert";
const std::string ShaderRegistry::FRAGMENT_SHADER_FILE_EXTENSION = ".frag";

std::map<std::string, Shader*> ShaderRegistry::mShadersByName;

bool ShaderRegistry::ExtractShaderInfo(const std::string& rShaderFileName, std::string& rShaderName, ShaderType& rShaderType)
{
	rShaderName = rShaderFileName;
	if (rShaderFileName.find(VERTEX_SHADER_FILE_EXTENSION) != std::string::npos)
	{
		rShaderType = ST_VERTEX;
		StringUtils::Replace(rShaderName, VERTEX_SHADER_FILE_EXTENSION, "");
		return true;
	} 
	else if (rShaderFileName.find(FRAGMENT_SHADER_FILE_EXTENSION) != std::string::npos)
	{
		rShaderType = ST_FRAGMENT;
		StringUtils::Replace(rShaderName, FRAGMENT_SHADER_FILE_EXTENSION, "");
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

		Shader* pShader;
		std::map<std::string, Shader*>::iterator it = mShadersByName.find(shaderName);
		if (it == mShadersByName.end())
		{
			pShader = new Shader(shaderName);
			mShadersByName.insert(std::make_pair(shaderName, pShader));
		}
		else 
		{
			pShader = it->second;
		}
		pShader->Compile(rShadersDirectory + "/" + shaderFiles[i], shaderType);
	}

	std::map<std::string, Shader*>::iterator it = mShadersByName.begin();
	while (it != mShadersByName.end())
	{
		it->second->Link();
		it++;
	}
}

Shader* ShaderRegistry::Find(const std::string& rShaderName)
{
	std::map<std::string, Shader*>::iterator it = mShadersByName.find(rShaderName);

	if (it == mShadersByName.end())
	{
		THROW_EXCEPTION(Exception, "Shader not found: %s", rShaderName.c_str());
	}

	return it->second;
}

void ShaderRegistry::Unload()
{
	std::map<std::string, Shader*>::iterator it = mShadersByName.begin();
	while (it != mShadersByName.end())
	{
		delete it->second;
		it++;
	}

	mShadersByName.clear();
}

#endif