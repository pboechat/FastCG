#if (!defined(SHADERREGISTRY_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define SHADERREGISTRY_H_

#include <Shader.h>

#include <map>
#include <string>

class ShaderRegistry
{
public:
	static void LoadShadersFromDisk(const std::string& rShadersDirectory);
	static ShaderPtr Find(const std::string& rShaderName);

private:
	static std::map<std::string, ShaderPtr> mShadersByName;

	ShaderRegistry()
	{
	}

	~ShaderRegistry()
	{
	}

	static bool ExtractShaderInfo(const std::string& rShaderFileName, std::string& rShaderName, ShaderType& rShaderType);

};

#endif