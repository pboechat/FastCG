#include <Material.h>
#include <Application.h>
#include <Geometry.h>

#include <glm/glm.hpp>
#include <vector>
#include <sstream>

#include <iostream>

#ifdef USE_OPENGL4

Material::Material(ShaderPtr shaderPtr) :
	mShaderPtr(shaderPtr)
{
}

Material::~Material()
{
}

void Material::Bind(const Geometry& rGeometry) const
{
	mShaderPtr->Bind();

	glm::mat4 model = rGeometry.GetModel();
	glm::mat4 view = Application::GetInstance()->GetMainCamera().GetView();
	glm::mat4 projection = Application::GetInstance()->GetMainCamera().GetProjection();
	glm::mat4 modelView = view * model;

	mShaderPtr->SetMat4("_Model", model);
	mShaderPtr->SetMat4("_View", view);
	mShaderPtr->SetMat4("_ModelView", modelView);
	mShaderPtr->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
	mShaderPtr->SetMat4("_Projection", projection);
	mShaderPtr->SetMat4("_ModelViewProjection", projection * modelView);

	mShaderPtr->SetVec4("_GlobalLightAmbientColor", Application::GetInstance()->GetGlobalAmbientLight());

	const std::vector<LightPtr>& rLights = Application::GetInstance()->GetLights();
	std::vector<LightPtr>::const_iterator it1 = rLights.begin();
	std::stringstream variableName;
	unsigned int c = 0;

	while (it1 != rLights.end())
	{
		LightPtr lightPtr = (*it1);
		glm::vec3 lightPosition = lightPtr->GetPosition();
		glm::vec4 lightAmbientColor = lightPtr->GetAmbientColor();
		glm::vec4 lightDiffuseColor = lightPtr->GetDiffuseColor();
		glm::vec4 lightSpecularColor = lightPtr->GetSpecularColor();

		variableName.str(std::string());
		variableName.clear();
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
		it1++;
	}

	std::map<std::string, float>::const_iterator it2 = mFloatParameters.begin();

	while (it2 != mFloatParameters.end())
	{
		mShaderPtr->SetFloat(it2->first, it2->second);
		it2++;
	}

	std::map<std::string, glm::vec4>::const_iterator it3 = mVec4Parameters.begin();

	while (it3 != mVec4Parameters.end())
	{
		mShaderPtr->SetVec4(it3->first, it3->second);
		it3++;
	}

	std::map<std::string, TexturePtr>::const_iterator it4 = mTextureParameters.begin();
	unsigned int textureUnit = 0;

	while (it4 != mTextureParameters.end())
	{
		mShaderPtr->SetTexture(it4->first, it4->second, textureUnit);
		textureUnit++;
		it4++;
	}
}

void Material::Unbind() const
{
	mShaderPtr->Unbind();

	// FIXME: shouldn't be necessary if we could garantee that all textures are unbinded after use!
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::SetFloat(const std::string& rParameterName, float value)
{
	mFloatParameters.insert(std::make_pair(rParameterName, value));
}

void Material::SetVec4(const std::string& rParameterName, const glm::vec4& rVector)
{
	mVec4Parameters.insert(std::make_pair(rParameterName, rVector));
}

void Material::SetTexture(const std::string& rParameterName, const TexturePtr& spTexture)
{
	mTextureParameters.insert(std::make_pair(rParameterName, spTexture));
}
#else

Material::Material() :
	mAmbientColor(1.0f, 1.0f, 1.0f, 1.0f),
	mDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
	mSpecularColor(1.0f, 1.0f, 1.0f, 1.0f),
	mEmissiveColor(1.0f, 1.0f, 1.0f, 1.0f),
	mShininess(3),
	mEmissive(false)
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