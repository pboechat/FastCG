#ifndef FASTCG_SHADER_SOURCE_H
#define FASTCG_SHADER_SOURCE_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <string>

namespace FastCG
{
	class ShaderSource
	{
	public:
		inline static std::string ParseFile(const std::string &rFileName, ShaderType shaderType, bool isIncluded = false);
		inline static void ParseSource(std::string &rSource, const std::string &rIncludePath, ShaderType shaderType, bool isIncluded = false);

	private:
		ShaderSource() = delete;
		~ShaderSource() = delete;
	};

}

#include <FastCG/Graphics/ShaderSource.inc>

#endif
