#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Pointer.h>
#include <Shader.h>
#include <Texture.h>

#include <string>
#include <map>

#include <glm/glm.hpp>

class Geometry;

#ifdef USE_OPENGL4
class Material
{
public:
	Material(ShaderPtr shaderPtr);
	~Material();

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
#else
class Material
{
public:
	Material();
	Material(const glm::vec4& ambientColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, const glm::vec4& emissiveColor, float shininess);
	~Material();

	inline const glm::vec4& GetAmbientColor() const
	{
		return mAmbientColor;
	}

	inline const glm::vec4& GetDiffuseColor() const
	{
		return mDiffuseColor;
	}

	inline const glm::vec4& GetSpecularColor() const
	{
		return mSpecularColor;
	}

	inline const glm::vec4& GetEmissiveColor() const
	{
		return mEmissiveColor;
	}

	inline float GetShininess() const
	{
		return mShininess;
	}

	inline bool IsEmissive() const
	{
		return mEmissive;
	}

	inline TexturePtr GetTexture() const
	{
		return mTexturePtr;
	}

	inline void SetAmbientColor(const glm::vec4& ambientColor)
	{
		mAmbientColor = ambientColor;
	}

	inline void SetDiffuseColor(const glm::vec4& diffuseColor)
	{
		mDiffuseColor = diffuseColor;
	}

	inline void SetSpecularColor(const glm::vec4& specularColor)
	{
		mSpecularColor = specularColor;
	}

	inline void SetEmissiveColor(const glm::vec4& emissiveColor)
	{
		mEmissiveColor = emissiveColor;
	}

	inline void SetShininess(float shininess)
	{
		mShininess = shininess;
	}

	inline void SetEmissive(bool emissive)
	{
		mEmissive = emissive;
	}

	inline void SetTexture(TexturePtr texturePtr)
	{
		mTexturePtr = texturePtr;
	}

	void Bind() const;
	
private:
	glm::vec4 mAmbientColor;
	glm::vec4 mDiffuseColor;
	glm::vec4 mSpecularColor;
	glm::vec4 mEmissiveColor;
	float mShininess;
	bool mEmissive;
	TexturePtr mTexturePtr;

};
#endif

typedef Pointer<Material> MaterialPtr;

#endif
