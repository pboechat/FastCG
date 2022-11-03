#ifndef FASTCG_BASE_MATERIAL_H
#define FASTCG_BASE_MATERIAL_H

#include <FastCG/ShaderConstants.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <cassert>

namespace FastCG
{
	template <class ShaderT>
	struct BaseMaterialArgs
	{
		std::string name;
		const ShaderT *pShader;
	};

	template <class BufferT, class ShaderT, class TextureT>
	class BaseMaterial
	{
	public:
		using Buffer = BufferT;
		using Shader = ShaderT;
		using Texture = TextureT;
		using MaterialArgs = BaseMaterialArgs<ShaderT>;

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline const Shader *GetShader() const
		{
			return mpShader;
		}

		inline const MaterialConstants &GetMaterialConstants() const
		{
			return mMaterialConstants;
		}

		inline const Buffer *GetMaterialConstantsBuffer() const
		{
			return mpMaterialConstantsBuffer;
		}

		inline void SetDiffuseColor(const glm::vec4 &diffuseColor)
		{
			mMaterialConstants.diffuseColor = diffuseColor;
		}

		inline void SetSpecularColor(const glm::vec4 &specularColor)
		{
			mMaterialConstants.specularColor = specularColor;
		}

		inline void SetColorMapTiling(const glm::vec2 &colorMapTiling)
		{
			mMaterialConstants.colorMapTiling = colorMapTiling;
		}

		inline void SetBumpMapTiling(const glm::vec2 &bumpMapTiling)
		{
			mMaterialConstants.bumpMapTiling = bumpMapTiling;
		}

		inline void SetShininess(float shininess)
		{
			mMaterialConstants.shininess = shininess;
		}

		inline void SetColorMap(const Texture *colorMap)
		{
			mpColorMap = colorMap;
		}

		inline const Texture *GetColorMap() const
		{
			return mpColorMap;
		}

		inline void SetBumpMap(const Texture *bumpMap)
		{
			mpBumpMap = bumpMap;
		}

		inline const Texture *GetBumpMap() const
		{
			return mpBumpMap;
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

	protected:
		const std::string mName;
		const Shader *mpShader;
		MaterialConstants mMaterialConstants{};
		const Buffer *mpMaterialConstantsBuffer{nullptr};
		const Texture *mpColorMap{nullptr};
		const Texture *mpBumpMap{nullptr};
		bool mTwoSided{false};
		bool mHasDepth{true};

		BaseMaterial(const MaterialArgs &rArgs, const Buffer *pMaterialConstantsBuffer) : mName(rArgs.name),
																						  mpShader(rArgs.pShader),
																						  mpMaterialConstantsBuffer(pMaterialConstantsBuffer)
		{
			assert(mpShader != nullptr);
			assert(pMaterialConstantsBuffer != nullptr);
		}
		virtual ~BaseMaterial() = default;
	};

}

#endif
