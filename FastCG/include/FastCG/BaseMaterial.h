#ifndef FASTCG_BASE_MATERIAL_H
#define FASTCG_BASE_MATERIAL_H

#include <FastCG/RenderingState.h>
#include <FastCG/BaseMaterialDefinition.h>
#include <FastCG/ConstantBuffer.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <cassert>

namespace FastCG
{
	template <class ShaderT, class TextureT>
	struct BaseMaterialArgs
	{
		std::string name;
		const BaseMaterialDefinition<ShaderT, TextureT> *pMaterialDefinition;
	};

	template <class BufferT, class ShaderT, class TextureT>
	class BaseMaterial
	{
	public:
		using Buffer = BufferT;
		using Shader = ShaderT;
		using Texture = TextureT;
		using MaterialArgs = BaseMaterialArgs<ShaderT, TextureT>;
		using MaterialDefinition = BaseMaterialDefinition<ShaderT, TextureT>;

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline const Shader *GetShader() const
		{
			return mpMaterialDefinition->GetShader();
		}

		inline const void *GetConstantBufferData() const
		{
			return mConstantBuffer.GetData();
		}

		inline uint32_t GetConstantBufferSize() const
		{
			return mConstantBuffer.GetSize();
		}

		inline const Buffer *GetConstantBuffer() const
		{
			return mpConstantBuffer;
		}

		inline bool GetConstant(const std::string &rName, float &rValue) const
		{
			return mConstantBuffer.GetMemberValue(rName, rValue);
		}

		inline bool GetConstant(const std::string &rName, glm::vec2 &rValue) const
		{
			return mConstantBuffer.GetMemberValue(rName, rValue);
		}

		inline bool GetConstant(const std::string &rName, glm::vec4 &rValue) const
		{
			return mConstantBuffer.GetMemberValue(rName, rValue);
		}

		inline bool SetConstant(const std::string &rName, float value)
		{
			return mConstantBuffer.SetMemberValue(rName, value);
		}

		inline bool SetConstant(const std::string &rName, const glm::vec2 &rValue)
		{
			return mConstantBuffer.SetMemberValue(rName, rValue);
		}

		inline bool SetConstant(const std::string &rName, const glm::vec4 &rValue)
		{
			return mConstantBuffer.SetMemberValue(rName, rValue);
		}

		inline size_t GetTextureCount() const
		{
			return mTextures.size();
		}

		inline void GetTextureAt(size_t i, std::string &rName, const Texture *&rpTexture) const
		{
			assert(i < mTextures.size());
			auto it = std::next(mTextures.cbegin(), i);
			rName = it->first;
			rpTexture = it->second;
		}

		inline bool GetTexture(const std::string &rName, const Texture *&rpTexture) const
		{
			auto it = mTextures.find(rName);
			if (it == mTextures.end())
			{
				assert(false);
				return false;
			}
			rpTexture = it->second;
			return true;
		}

		inline bool SetTexture(const std::string &rName, const Texture *pTexture)
		{
			auto it = mTextures.find(rName);
			if (it == mTextures.end())
			{
				assert(false);
				return false;
			}
			it->second = pTexture;
			return true;
		}

		inline const RenderingState &GetRenderingState() const
		{
			return mpMaterialDefinition->GetRenderingState();
		}

	protected:
		std::string mName;
		const MaterialDefinition *mpMaterialDefinition;
		const Buffer *mpConstantBuffer;
		ConstantBuffer mConstantBuffer;
		std::unordered_map<std::string, const Texture *> mTextures;

		BaseMaterial(const MaterialArgs &rArgs, const Buffer *pConstantsBuffer) : mName(rArgs.name),
																				  mpMaterialDefinition(rArgs.pMaterialDefinition),
																				  mpConstantBuffer(pConstantsBuffer),
																				  mConstantBuffer(rArgs.pMaterialDefinition->GetConstantBuffer()),
																				  mTextures(rArgs.pMaterialDefinition->GetTextures())
		{
			assert(mpMaterialDefinition != nullptr);
		}
		virtual ~BaseMaterial() = default;
	};

}

#endif
