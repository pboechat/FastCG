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
		ST_VERTEX = 0,
		ST_FRAGMENT,
		ST_MAX
	};

	using ShaderTypeInt = std::underlying_type_t<ShaderType>;

	template <typename T>
	using ShaderTypeValueArray = std::array<T, (ShaderTypeInt)ShaderType::ST_MAX>;

	struct ShaderArgs
	{
		std::string name;
		ShaderTypeValueArray<std::string> shaderFileNames;
	};

	class BaseShader
	{
	public:
		static constexpr char *const VERTEX_SHADER_FILE_EXTENSION = ".vert";
		static constexpr char *const FRAGMENT_SHADER_FILE_EXTENSION = ".frag";
		static constexpr uint32_t VERTICES_ATTRIBUTE_INDEX = 0;
		static constexpr uint32_t NORMALS_ATTRIBUTE_INDEX = 1;
		static constexpr uint32_t UVS_ATTRIBUTE_INDEX = 2;
		static constexpr uint32_t TANGENTS_ATTRIBUTE_INDEX = 3;
		static constexpr uint32_t COLORS_ATTRIBUTE_INDEX = 4;
		static constexpr uint32_t SCENE_CONSTANTS_BINDING_INDEX = 0;
		static constexpr uint32_t INSTANCE_CONTANTS_BINDING_INDEX = 1;
		static constexpr uint32_t LIGHTING_CONSTANTS_BINDING_INDEX = 2;
		static constexpr uint32_t SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_BINDING_INDEX = 0x10;

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
