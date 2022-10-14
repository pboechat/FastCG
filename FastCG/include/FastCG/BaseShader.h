#ifndef FASTCG_BASE_SHADER_H
#define FASTCG_BASE_SHADER_H

#include <FastCG/Texture.h>

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <cstdint>

namespace FastCG
{
	enum class ShaderType : uint8_t
	{
		ST_VERTEX = 0,
		ST_FRAGMENT
	};

	class BaseShader
	{
	public:
		static const unsigned int VERTICES_ATTRIBUTE_INDEX = 0;
		static const unsigned int NORMALS_ATTRIBUTE_INDEX = 1;
		static const unsigned int UVS_ATTRIBUTE_INDEX = 2;
		static const unsigned int TANGENTS_ATTRIBUTE_INDEX = 3;
		static const unsigned int COLORS_ATTRIBUTE_INDEX = 4;

		BaseShader(const std::string &rName) : mName(rName)
		{
		}
		virtual ~BaseShader() = default;

		inline const std::string &GetName() const
		{
			return mName;
		}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void BindAttributeLocation(const std::string &rAttribute, unsigned int location) const = 0;
		virtual void SetInt(const std::string &rParameterName, int value) const = 0;
		virtual void SetFloat(const std::string &rParameterName, float value) const = 0;
		virtual void SetBool(const std::string &rParameterName, bool value) const = 0;
		virtual void SetVec2(const std::string &rParameterName, float x, float y) const = 0;
		virtual void SetVec2(const std::string &rParameterName, const glm::vec2 &rVector) const = 0;
		virtual void SetVec3(const std::string &rParameterName, float x, float y, float z) const = 0;
		virtual void SetVec3(const std::string &rParameterName, const glm::vec3 &rVector) const = 0;
		virtual void SetVec3Array(const std::string &rParameterName, unsigned int arraySize, const glm::vec3 *pVectorArray) const = 0;
		virtual void SetVec4(const std::string &rParameterName, float x, float y, float z, float w) const = 0;
		virtual void SetVec4(const std::string &rParameterName, const glm::vec4 &rVector) const = 0;
		virtual void SetMat3(const std::string &rParameterName, const glm::mat3 &rMatrix) const = 0;
		virtual void SetMat4(const std::string &rParameterName, const glm::mat4 &rMatrix) const = 0;
		virtual void SetTexture(const std::string &rParameterName, const std::shared_ptr<Texture> &pTexture, int32_t textureUnit) const = 0;
		virtual void SetTexture(const std::string &rParameterName, unsigned int textureId, int32_t textureUnit) const = 0;
		virtual void SetTexture(const std::string &rParameterName, int32_t textureUnit) const = 0;
		virtual void Compile(const std::string &rShaderFileName, ShaderType shaderType) = 0;
		virtual void Link() = 0;

	private:
		std::string mName;
	};

}

#endif
