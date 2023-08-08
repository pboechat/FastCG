#ifndef FASTCG_BASE_SHADER_H
#define FASTCG_BASE_SHADER_H

#include <FastCG/Graphics/GraphicsUtils.h>

#include <type_traits>
#include <string>
#include <array>

namespace FastCG
{
	using ShaderTypeInt = std::underlying_type_t<ShaderType>;

	template <typename T>
	using ShaderTypeValueArray = std::array<T, (ShaderTypeInt)ShaderType::LAST>;

	struct ShaderProgramData
	{
		size_t dataSize{0};
		const void *pData{nullptr};
	};

	struct ShaderArgs
	{
		std::string name;
		ShaderTypeValueArray<ShaderProgramData> programsData;
		bool text{false};
	};

	class BaseShader
	{
	public:
		inline const std::string &GetName() const
		{
			return mName;
		}

	protected:
		const std::string mName;

		BaseShader(const ShaderArgs &rArgs) : mName(rArgs.name) {}
		virtual ~BaseShader() = default;
	};

}

#endif
