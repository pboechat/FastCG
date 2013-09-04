#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Shader.h>
#include <Texture.h>
#include <Exception.h>

#include <glm/glm.hpp>

#include <string>
#include <map>

class Material
{
public:
	Material(Shader* pShader);
	~Material();

	inline Shader* GetShader() const
	{
		return mpShader;
	}

	inline void SetShader(Shader* pShader)
	{
		mpShader = pShader;
	}

	void SetUpParameters() const;

	float GetFloat(const std::string& rParameterName) const
	{
		std::map<std::string, float>::const_iterator it = mFloatParameters.find(rParameterName);
		if (it == mFloatParameters.end())
		{
			THROW_EXCEPTION(Exception, "Parameter not found: %s", rParameterName.c_str());
		}
		return it->second;
	}

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

	inline void SetTexture(const std::string& rParameterName, Texture* pTexture)
	{
		mTextureParameters[rParameterName] = pTexture;
	}

	inline const glm::vec2& GetTextureTiling(const std::string& rTextureName) const
	{
		return mTexturesTiling.find(rTextureName)->second;
	}

	inline void SetTextureTiling(const std::string& rTextureName, const glm::vec2& rTextureTiling)
	{
		mTexturesTiling[rTextureName] = rTextureTiling;
	}

	inline bool IsUnlit() const
	{
		return mUnlit;
	}

	inline void SetUnlit(bool unlit)
	{
		mUnlit = unlit;
	}

	inline bool IsTwoSided() const
	{
		return mTwoSided;
	}

	inline void SetTwoSided(bool twoSided)
	{
		mTwoSided = twoSided;
	}
	
private:
	Shader* mpShader;
	std::map<std::string, float> mFloatParameters;
	std::map<std::string, glm::vec4> mVec4Parameters;
	std::map<std::string, glm::mat4> mMat4Parameters;
	std::map<std::string, Texture*> mTextureParameters;
	std::map<std::string, glm::vec2> mTexturesTiling;
	bool mUnlit;
	bool mTwoSided;

};

#endif
