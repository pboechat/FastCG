/*
 * Material.cpp
 *
 *  Created on: 10/04/2013
 *      Author: pboechat
 */

#include <Material.h>

Material::Material(Shader* pShader)
	: mpShader(pShader)
{
}

Material::~Material()
{
}

void Material::Bind()
{
	mpShader->Bind();

	// TODO: parameter passing code
}

void Material::Unbind()
{
	mpShader->Unbind();
}

void Material::SetFloat(const std::string& arg0, float arg1)
{
	mFloatParameters.insert(std::make_pair(arg0, arg1));
}
