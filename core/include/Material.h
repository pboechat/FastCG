#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Shader.h"

#include <string>
#include <map>

class Material
{
public:
	Material(Shader* pShader);
	virtual ~Material();

	void Bind();
	void Unbind();
	void SetFloat(const std::string& arg0, float arg1);

private:
	Shader* mpShader;
	std::map<std::string, float> mFloatParameters;

};

#endif
