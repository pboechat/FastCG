#if (!defined(SHADERREGISTRY_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define SHADERREGISTRY_H_

#include <Shader.h>

#include <map>
#include <string>

class ShaderRegistry
{
public:
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