#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Pointer.h>
#include <Shader.h>

#include <string>
#include <map>

#include <glm/glm.hpp>

class Geometry;

class Material
{
public:
	Material(ShaderPtr shaderPtr);
	virtual ~Material();

	inline ShaderPtr GetShader()
	{
		return mShaderPtr;
	}

	void Bind(const Geometry& rGeometry) const;
	void Unbind() const;
	void SetFloat(const std::string& rParameterName, float value);
	void SetColor(const std::string& rParameterName, const glm::vec4& value);

private:
	ShaderPtr mShaderPtr;
	std::map<std::string, float> mFloatParameters;
	std::map<std::string, glm::vec4> mColorParameters;

};

typedef Pointer<Material> MaterialPtr;

#endif
