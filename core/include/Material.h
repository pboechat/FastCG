#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Pointer.h>
#include <Shader.h>
#include <Texture.h>

#include <string>
#include <map>

#include <glm/glm.hpp>

#ifdef USE_PROGRAMMABLE_PIPELINE

class Material
{
public:
	Material(ShaderPtr shaderPtr);
	~Material();

	inline ShaderPtr GetShader() const
	{
		return mShaderPtr;
	}

	inline void SetShader(ShaderPtr shaderPtr)
	{
		mShaderPtr = shaderPtr;
	}

	void Bind(const glm::mat4& rModel) const;
	void Unbind() const;

	inline void SetFloat(const std::string& rParameterName, float value)
	{
		mFloatParameters[rParameterName] = value;
	}

	inline void SetVec4(const std::string& rParameterName, const glm::vec4& rVector)
	{
		mVec4Parameters[rParameterName] = rVector;
	}

	inline void SetMat4(const std::string& rParameterName, const glm::mat4& rMatrix)
	{
		mMat4Parameters[rParameterName] = rMatrix;
	}

	inline void SetTexture(const std::string& rParameterName, const TexturePtr& texturePtr)
	{
		mTextureParameters[rParameterName] = texturePtr;
	}
	
	inline const glm::vec2& GetTextureTiling(const std::string& rTextureName) const
	{
		return mTexturesTiling.find(rTextureName)->second;
	}

	inline void SetTextureTiling(const std::string& rTextureName, const glm::vec2& rTextureTiling)
	{
		mTexturesTiling[rTextureName] = rTextureTiling;
	}

private:
	ShaderPtr mShaderPtr;
	std::map<std::string, float> mFloatParameters;
	std::map<std::string, glm::vec4> mVec4Parameters;
	std::map<std::string, glm::mat4> mMat4Parameters;
	std::map<std::string, TexturePtr> mTextureParameters;
	std::map<std::string, glm::vec2> mTexturesTiling;

};

#else

class Material
{
public:
	Material(const glm::vec4& ambientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
		     const glm::vec4& diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
			 const glm::vec4& specularColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
			 float shininess = 3, 
			 bool emissive = false, 
			 const glm::vec4& emissiveColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
