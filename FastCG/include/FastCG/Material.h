#ifndef FASTCG_MATERIAL_H
#define FASTCG_MATERIAL_H

#include <FastCG/Shader.h>
#include <FastCG/Texture.h>
#include <FastCG/Exception.h>

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <map>
#include <cassert>

namespace FastCG
{
	class Material
	{
	public:
		Material(const std::shared_ptr<Shader> &pShader);

		inline const std::shared_ptr<Shader> &GetShader() const
		{
			return mpShader;
		}

		inline void SetShader(const std::shared_ptr<Shader> &pShader)
		{
			mpShader = pShader;
		}

		void SetUpParameters() const;

		inline float GetFloat(const std::string &rParameterName) const
		{
			auto it = mFloatParameters.find(rParameterName);
			assert(it != mFloatParameters.end());
			return it->second;
		}

		inline void SetFloat(const std::string &rParameterName, float value)
		{
			mFloatParameters[rParameterName] = value;
		}

		inline void SetVec4(const std::string &rParameterName, const glm::vec4 &rVector)
		{
			mVec4Parameters[rParameterName] = rVector;
		}

		inline void SetMat4(const std::string &rParameterName, const glm::mat4 &rMatrix)
		{
			mMat4Parameters[rParameterName] = rMatrix;
		}

		inline void SetTexture(const std::string &rParameterName, const std::shared_ptr<Texture> &pTexture)
		{
			mTextureParameters[rParameterName] = pTexture;
		}

		inline const glm::vec2 &GetTextureTiling(const std::string &rTextureName) const
		{
			return mTexturesTiling.find(rTextureName)->second;
		}

		inline void SetTextureTiling(const std::string &rTextureName, const glm::vec2 &rTextureTiling)
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

		inline bool HasDepth() const
		{
			return mHasDepth;
		}

		inline void SetHasDepth(bool hasDepth)
		{
			mHasDepth = hasDepth;
		}

	private:
		std::shared_ptr<Shader> mpShader;
		std::map<std::string, float> mFloatParameters;
		std::map<std::string, glm::vec4> mVec4Parameters;
		std::map<std::string, glm::mat4> mMat4Parameters;
		std::map<std::string, std::shared_ptr<Texture>> mTextureParameters;
		std::map<std::string, glm::vec2> mTexturesTiling;
		bool mUnlit{false};
		bool mTwoSided{false};
		bool mHasDepth{true};
	};

}

#endif
