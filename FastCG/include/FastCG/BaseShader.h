#ifndef FASTCG_BASE_SHADER_H
#define FASTCG_BASE_SHADER_H

#include <type_traits>
#include <string>
#include <array>
#include <cstdint>

namespace FastCG
{
	enum class ShaderType : uint8_t
	{
		VERTEX = 0,
		FRAGMENT,
		MAX
	};

	using ShaderTypeInt = std::underlying_type_t<ShaderType>;

	template <typename T>
	using ShaderTypeValueArray = std::array<T, (ShaderTypeInt)ShaderType::MAX>;

	template <ShaderType ShaderTypeT>
	struct ShaderFileExtension;

#define FASTCG_DECLARE_SHADER_FILE_EXTENSION(shaderType, ext) \
	template <>                                               \
	struct ShaderFileExtension<shaderType>                    \
	{                                                         \
		static const char *value()                            \
		{                                                     \
			return ext;                                       \
		}                                                     \
	}

	FASTCG_DECLARE_SHADER_FILE_EXTENSION(ShaderType::VERTEX, ".vert");
	FASTCG_DECLARE_SHADER_FILE_EXTENSION(ShaderType::FRAGMENT, ".frag");

	struct ShaderArgs
	{
		std::string name;
		ShaderTypeValueArray<std::string> shaderFileNames;
	};

	class BaseShader
	{
	public:
		inline const std::string &GetName() const
		{
			return mArgs.name;
		}

	protected:
		const ShaderArgs mArgs;

		BaseShader(const ShaderArgs &rArgs) : mArgs(rArgs) {}
		virtual ~BaseShader() = default;
	};

}

#endif
