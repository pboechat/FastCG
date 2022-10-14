#ifndef FASTCG_SHADER_SOURCE_H_
#define FASTCG_SHADER_SOURCE_H_

#include <string>

namespace FastCG
{
	class ShaderSource
	{
	public:
		static std::string Parse(const std::string &rFileName);

	private:
		static std::string s_mShaderFolderPath;

		ShaderSource() = delete;
		~ShaderSource() = delete;
	};

}

#endif
