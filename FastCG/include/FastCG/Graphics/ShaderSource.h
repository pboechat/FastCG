#ifndef FASTCG_SHADER_SOURCE_H
#define FASTCG_SHADER_SOURCE_H

#include <string>

namespace FastCG
{
	class ShaderSource
	{
	public:
		inline static std::string ParseFile(const std::string &rFileName);
		inline static void ParseSource(std::string &rSource, const std::string &rIncludePath);

	private:
		ShaderSource() = delete;
		~ShaderSource() = delete;
	};

}

#include <FastCG/Graphics/ShaderSource.inc>

#endif
