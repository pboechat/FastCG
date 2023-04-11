#ifndef FASTCG_BASE_MATERIAL_H
#define FASTCG_BASE_MATERIAL_H

#include <FastCG/Rendering/MaterialDefinition.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/GraphicsContextState.h>
#include <FastCG/Graphics/ConstantBuffer.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>

namespace FastCG
{
	struct MaterialArgs
	{
		std::string name;
		std::shared_ptr<MaterialDefinition> pMaterialDefinition;
	};

	class Material final
	{
	public:
		Material(const MaterialArgs &rArgs);
		~Material();

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline const std::shared_ptr<MaterialDefinition> &GetMaterialDefinition() const
		{
			return mpMaterialDefinition;
		}

		inline const Shader *GetShader() const
		{
			return mpMaterialDefinition->GetShader();
		}

		inline const uint8_t *GetConstantBufferData() const
		{
			return mConstantBuffer.GetData();
		}

		inline void SetConstantBufferData(const uint8_t *pData, size_t offset, size_t size)
		{
			mConstantBuffer.SetData(pData, offset, size);
		}

		inline uint32_t GetConstantBufferSize() const
		{
			return mConstantBuffer.GetSize();
		}

		inline const Buffer *GetConstantBuffer() const
		{
			return mpConstantBuffer;
		}

		inline size_t GetConstantCount() const
		{
			return mConstantBuffer.GetMembers().size();
		}

		inline const auto &GetConstantAt(size_t i) const
		{
			assert(i < mConstantBuffer.GetMembers().size());
			return mConstantBuffer.GetMembers()[i];
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

		inline const GraphicsContextState &GetGraphicsContextState() const
		{
			return mpMaterialDefinition->GetGraphicsContextState();
		}

	protected:
		std::string mName;
		const std::shared_ptr<MaterialDefinition> mpMaterialDefinition;
		const Buffer *mpConstantBuffer;
		ConstantBuffer mConstantBuffer;
		std::unordered_map<std::string, const Texture *> mTextures;
	};

}

#endif
