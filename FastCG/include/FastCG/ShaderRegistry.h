#ifndef FASTCG_SHADER_REGISTRY_H_
#define FASTCG_SHADER_REGISTRY_H_

#include <FastCG/Shader.h>

#include <memory>
#include <map>
#include <string>

namespace FastCG
{
	class ShaderRegistry
	{
	public:
		static void LoadShadersFromDisk(const std::string &rShadersDirectory);
		static std::shared_ptr<Shader> Find(const std::string &rShaderName);
		static void Unload();

	private:
		static std::map<std::string, std::shared_ptr<Shader>> mShadersByName;

		ShaderRegistry() = delete;
		~ShaderRegistry() = delete;

		static bool ExtractShaderInfo(const std::string &rShaderFileName, std::string &rShaderName, ShaderType &rShaderType);
	};

}

#endif