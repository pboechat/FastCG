#include <FastCG/Mesh.h>
#include <FastCG/Material.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <cstdint>

namespace FastCG
{
	Material::Material(const std::shared_ptr<Shader> &pShader) : mpShader(pShader)
	{
	}

	void Material::SetUpParameters() const
	{
		for (auto it = mFloatParameters.begin(); it != mFloatParameters.end(); it++)
		{
			mpShader->SetFloat(it->first, it->second);
		}

		for (auto it = mVec4Parameters.begin(); it != mVec4Parameters.end(); it++)
		{
			mpShader->SetVec4(it->first, it->second);
		}

		for (auto it = mMat4Parameters.begin(); it != mMat4Parameters.end(); it++)
		{
			mpShader->SetMat4(it->first, it->second);
		}

		GLint textureUnit = 0;
		for (auto it = mTextureParameters.begin(); it != mTextureParameters.end(); it++)
		{
			mpShader->SetTexture(it->first, it->second, textureUnit);

			std::stringstream variableName;
			variableName << it->first << "Tiling";

			glm::vec2 tiling(1, 1);
			auto it2 = mTexturesTiling.find(it->first);
			if (it2 != mTexturesTiling.end())
			{
				tiling = it2->second;
			}

			mpShader->SetVec2(variableName.str(), tiling);

			textureUnit++;
		}
	}

}
