#include <Material.h>
#include <Application.h>
#include <Mesh.h>
#include <Transform.h>

#include <GL/gl.h>

#include <vector>
#include <sstream>
#include <iostream>

#ifdef USE_PROGRAMMABLE_PIPELINE

Material::Material(ShaderPtr shaderPtr) :
	mShaderPtr(shaderPtr)
{
}

Material::~Material()
{
}

void Material::SetUpShaderParameters() const
{
	std::map<std::string, float>::const_iterator floatParametersCursor = mFloatParameters.begin();

	while (floatParametersCursor != mFloatParameters.end())
	{
		mShaderPtr->SetFloat(floatParametersCursor->first, floatParametersCursor->second);
		floatParametersCursor++;
	}

	std::map<std::string, glm::vec4>::const_iterator vec4ParametersCursor = mVec4Parameters.begin();

	while (vec4ParametersCursor != mVec4Parameters.end())
	{
		mShaderPtr->SetVec4(vec4ParametersCursor->first, vec4ParametersCursor->second);
		vec4ParametersCursor++;
	}

	std::map<std::string, glm::mat4>::const_iterator mat4ParametersCursor = mMat4Parameters.begin();

	while (mat4ParametersCursor != mMat4Parameters.end())
	{
		mShaderPtr->SetMat4(mat4ParametersCursor->first, mat4ParametersCursor->second);
		mat4ParametersCursor++;
	}

	std::map<std::string, TexturePtr>::const_iterator textureParametersCursor = mTextureParameters.begin();
	unsigned int textureUnit = 0;

	while (textureParametersCursor != mTextureParameters.end())
	{
		mShaderPtr->SetTexture(textureParametersCursor->first, textureParametersCursor->second, textureUnit);

		std::stringstream variableName;
		variableName << textureParametersCursor->first << "Tiling";

		std::map<std::string, glm::vec2>::const_iterator textureTilingCursor = mTexturesTiling.find(textureParametersCursor->first);
		glm::vec2 tiling(1.0f, 1.0f);
		if (textureTilingCursor != mTexturesTiling.end())
		{
			tiling = textureTilingCursor->second;
		}

		mShaderPtr->SetVec2(variableName.str(), tiling);

		textureUnit++;
		textureParametersCursor++;
	}
}

#else

Material::Material(const glm::vec4& ambientColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, float shininess, bool emissive, const glm::vec4& emissiveColor) :
	mAmbientColor(ambientColor),
	mDiffuseColor(diffuseColor),
	mSpecularColor(specularColor),
	mShininess(shininess),
	mEmissive(emissive),
	mEmissiveColor(emissiveColor)
{
}

Material::~Material()
{
}

void Material::SetUpShaderParameters() const
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, &mAmbientColor[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &mDiffuseColor[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &mSpecularColor[0]);
	if (mEmissive)
	{
		glMaterialfv(GL_FRONT, GL_EMISSION, &mEmissiveColor[0]);
	}
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

	if (mTexturePtr != 0)
	{
		mTexturePtr->Bind();
	}
}

#endif