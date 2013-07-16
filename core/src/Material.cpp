#include <Material.h>
#include <Mesh.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <sstream>
#include <iostream>

Material::Material(Shader* pShader) :
	mpShader(pShader),
	mUnlit(false),
	mTwoSided(false)
{
}

Material::~Material()
{
}

void Material::SetUpParameters() const
{
	std::map<std::string, float>::const_iterator floatParametersCursor = mFloatParameters.begin();

	while (floatParametersCursor != mFloatParameters.end())
	{
		mpShader->SetFloat(floatParametersCursor->first, floatParametersCursor->second);
		floatParametersCursor++;
	}

	std::map<std::string, glm::vec4>::const_iterator vec4ParametersCursor = mVec4Parameters.begin();

	while (vec4ParametersCursor != mVec4Parameters.end())
	{
		mpShader->SetVec4(vec4ParametersCursor->first, vec4ParametersCursor->second);
		vec4ParametersCursor++;
	}

	std::map<std::string, glm::mat4>::const_iterator mat4ParametersCursor = mMat4Parameters.begin();

	while (mat4ParametersCursor != mMat4Parameters.end())
	{
		mpShader->SetMat4(mat4ParametersCursor->first, mat4ParametersCursor->second);
		mat4ParametersCursor++;
	}

	std::map<std::string, Texture*>::const_iterator textureParametersCursor = mTextureParameters.begin();
	unsigned int textureUnit = 0;

	while (textureParametersCursor != mTextureParameters.end())
	{
		mpShader->SetTexture(textureParametersCursor->first, textureParametersCursor->second, textureUnit);

		std::stringstream variableName;
		variableName << textureParametersCursor->first << "Tiling";

		std::map<std::string, glm::vec2>::const_iterator textureTilingCursor = mTexturesTiling.find(textureParametersCursor->first);
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