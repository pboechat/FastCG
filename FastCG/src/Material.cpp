#include <FastCG/Mesh.h>
#include <FastCG/Material.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <cstdint>

namespace FastCG
{
	Material::Material(const std::shared_ptr<Shader>& pShader) :
		mpShader(pShader)
	{
	}

	void Material::SetUpParameters() const
	{
		auto floatParametersCursor = mFloatParameters.begin();
		while (floatParametersCursor != mFloatParameters.end())
		{
			mpShader->SetFloat(floatParametersCursor->first, floatParametersCursor->second);
			floatParametersCursor++;
		}

		auto vec4ParametersCursor = mVec4Parameters.begin();
		while (vec4ParametersCursor != mVec4Parameters.end())
		{
			mpShader->SetVec4(vec4ParametersCursor->first, vec4ParametersCursor->second);
			vec4ParametersCursor++;
		}

		auto mat4ParametersCursor = mMat4Parameters.begin();
		while (mat4ParametersCursor != mMat4Parameters.end())
		{
			mpShader->SetMat4(mat4ParametersCursor->first, mat4ParametersCursor->second);
			mat4ParametersCursor++;
		}

		auto textureParametersCursor = mTextureParameters.begin();
		GLint textureUnit = 0;
		while (textureParametersCursor != mTextureParameters.end())
		{
			mpShader->SetTexture(textureParametersCursor->first, textureParametersCursor->second, textureUnit);

			std::stringstream variableName;
			variableName << textureParametersCursor->first << "Tiling";

			auto textureTilingCursor = mTexturesTiling.find(textureParametersCursor->first);
			glm::vec2 tiling(1.0f, 1.0f);
			if (textureTilingCursor != mTexturesTiling.end())
			{
				tiling = textureTilingCursor->second;
			}

			mpShader->SetVec2(variableName.str(), tiling);

			textureUnit++;
			textureParametersCursor++;
		}
	}

}

