#ifndef SHADERREGISTRY_H_
#define SHADERREGISTRY_H_

#include <Shader.h>

#include <map>
#include <string>

class ShaderRegistry
{
public:
	static const std::string VERTEX_SHADER_FILE_EXTENSION;
	static const std::string FRAGMENT_SHADER_FILE_EXTENSION;

	static void LoadShadersFromDisk(const std::string& rShadersDirectory);
	static Shader* Find(const std::string& rShaderName);
	static void Unload();

private:
	static std::map<std::string, Shader*> mShadersByName;

	ShaderRegistry()
	{
	}

	~ShaderRegistry()
	{
	}

	static bool ExtractShaderInfo(const std::string& rShaderFileName, std::string& rShaderName, ShaderType& rShaderType);

};

#endif