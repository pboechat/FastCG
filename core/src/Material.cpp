#include <Material.h>
#include <Application.h>
#include <Geometry.h>

#include <glm/glm.hpp>

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
	mShaderPtr->SetMat4("_Projection", projection);
	mShaderPtr->SetMat4("_ModelViewProjection", projection * modelView);

	std::map<std::string, float>::const_iterator it1 = mFloatParameters.begin();
	while (it1 != mFloatParameters.end())
	{
		mShaderPtr->SetFloat(it1->first, it1->second);
		it1++;
	}

	std::map<std::string, glm::vec4>::const_iterator it2 = mColorParameters.begin();
	while (it2 != mColorParameters.end())
	{
		mShaderPtr->SetVec4(it2->first, it2->second);
		it2++;
	}
}

void Material::Unbind() const
{
	mShaderPtr->Unbind();
}

void Material::SetFloat(const std::string& rParameterName, float value)
{
	mFloatParameters.insert(std::make_pair(rParameterName, value));
}

void Material::SetColor(const std::string& rParameterName, const glm::vec4& value)
{
	mColorParameters.insert(std::make_pair(rParameterName, value));
}
