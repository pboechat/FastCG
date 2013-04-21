#include <Material.h>
#include <Application.h>
#include <Geometry.h>

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

void Material::Bind(const glm::mat4& rModel) const
{
	mShaderPtr->Bind();

	glm::mat4 view = Application::GetInstance()->GetMainCamera()->GetView();
	glm::mat4 projection = Application::GetInstance()->GetMainCamera()->GetProjection();
	glm::mat4 modelView = view * rModel;

	mShaderPtr->SetMat4("_Model", rModel);
	mShaderPtr->SetMat4("_View", view);
	mShaderPtr->SetMat4("_ModelView", modelView);
	mShaderPtr->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
	mShaderPtr->SetMat4("_Projection", projection);
	mShaderPtr->SetMat4("_ModelViewProjection", projection * modelView);
	mShaderPtr->SetVec4("_GlobalLightAmbientColor", Application::GetInstance()->GetGlobalAmbientLight());

	const std::vector<LightPtr>& rLights = Application::GetInstance()->GetLights();
	std::vector<LightPtr>::const_iterator lightsCursor = rLights.begin();
	unsigned int c = 0;
	while (lightsCursor != rLights.end())
	{
		LightPtr lightPtr = (*lightsCursor);
		glm::vec3 lightPosition = lightPtr->GetPosition();
		glm::vec4 lightAmbientColor = lightPtr->GetAmbientColor();
		glm::vec4 lightDiffuseColor = lightPtr->GetDiffuseColor();
		glm::vec4 lightSpecularColor = lightPtr->GetSpecularColor();

		std::stringstream variableName;
		variableName << "_Light" << c << "Position";
		mShaderPtr->SetVec3(variableName.str(), lightPosition);

		variableName.str(std::string());
		variableName.clear();
		variableName << "_Light" << c << "AmbientColor";
		mShaderPtr->SetVec4(variableName.str(), lightAmbientColor);

		variableName.str(std::string());
		variableName.clear();
		variableName << "_Light" << c << "DiffuseColor";
		mShaderPtr->SetVec4(variableName.str(), lightDiffuseColor);

		variableName.str(std::string());
		variableName.clear();
		variableName << "_Light" << c << "SpecularColor";
		mShaderPtr->SetVec4(variableName.str(), lightSpecularColor);

		c++;
		lightsCursor++;
	}

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

		std::map<std::string, glm::vec2>::const_iterator it5 = mTexturesTiling.find(textureParametersCursor->first);
		glm::vec2 tiling(1.0f, 1.0f);
		if (it5 != mTexturesTiling.end())
		{
			tiling = it5->second;
		}

		mShaderPtr->SetVec2(variableName.str(), tiling);

		textureUnit++;
		textureParametersCursor++;
	}
}

void Material::Unbind() const
{
	mShaderPtr->Unbind();

	// FIXME: shouldn't be necessary if we could guarantee that all textures are unbound after use!
	glBindTexture(GL_TEXTURE_2D, 0);
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

void Material::Bind() const
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