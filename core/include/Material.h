#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Pointer.h>
#include <Shader.h>
#include <Texture.h>

#include <string>
#include <map>

#include <glm/glm.hpp>

class Geometry;

class Material
{
public:
	Material(ShaderPtr shaderPtr);
	virtual ~Material();

	void Bind(const Geometry& rGeometry) const;
	void Unbind() const;
	void SetFloat(const std::string& rParameterName, float value);
	void SetVec4(const std::string& rParameterName, const glm::vec4& rVector);
	void SetTexture(const std::string& rParameterName, const TexturePtr& spTexture);

private:
	ShaderPtr mShaderPtr;
	std::map<std::string, float> mFloatParameters;
	std::map<std::string, glm::vec4> mVec4Parameters;
	std::map<std::string, TexturePtr> mTextureParameters;

};

typedef Pointer<Material> MaterialPtr;

#endif
