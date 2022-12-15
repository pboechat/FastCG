#ifndef FASTCG_BASE_MATERIAL_H
#define FASTCG_BASE_MATERIAL_H

#include <FastCG/ShaderConstants.h>
#include <FastCG/RenderingEnums.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

#include <cassert>
#include <array>

namespace FastCG
{
	struct StencilFaceFunc
	{
		Face face;
		CompareOp stencilFunc;
		int32_t ref;
		uint32_t mask;
	};

	struct StencilFaceOp
	{
		Face face;
		StencilOp stencilFail;
		StencilOp depthFail;
		StencilOp depthPass;
	};

	struct BlendFuncs
	{
		BlendFunc color;
		BlendFunc alpha;
	};

	struct BlendFactors
	{
		BlendFactor srcColor;
		BlendFactor dstColor;
		BlendFactor srcAlpha;
		BlendFactor dstAlpha;
	};

	template <class ShaderT, class TextureT>
	struct BaseMaterialArgs
	{
		std::string name;
		const ShaderT *pShader;
		MaterialConstants materialConstants{};
		const TextureT *pColorMap{nullptr};
		const TextureT *pBumpMap{nullptr};
		bool depthTest{true};
		bool depthWrite{true};
		CompareOp depthFunc{CompareOp::LEQUAL};
		bool scissorTest{false};
		bool stencilTest{false};
		std::array<StencilFaceFunc, (size_t)Face::MAX> stencilFuncs{};
		std::array<StencilFaceOp, (size_t)Face::MAX> stencilOps{};
		uint32_t stencilWriteMask{0};
		Face cullMode{Face::BACK};
		bool blend{false};
		BlendFuncs blendFuncs{};
		BlendFactors blendFactors{};
	};

	template <class BufferT, class ShaderT, class TextureT>
	class BaseMaterial
	{
	public:
		using Buffer = BufferT;
		using Shader = ShaderT;
		using Texture = TextureT;
		using MaterialArgs = BaseMaterialArgs<ShaderT, TextureT>;

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

		inline auto GetDepthTest() const
		{
			return mDepthTest;
		}

		inline auto GetDepthWrite() const
		{
			return mDepthWrite;
		}

		inline auto GetDepthFunc() const
		{
			return mDepthFunc;
		}

		inline auto GetScissorTest() const
		{
			return mScissorTest;
		}

		inline auto GetStencilTest() const
		{
			return mStencilTest;
		}

		inline const auto &GetStencilFuncs() const
		{
			return mStencilFuncs;
		}

		inline auto GetStencilWriteMask() const
		{
			return mStencilWriteMask;
		}

		inline const auto &GetStencilOps() const
		{
			return mStencilOps;
		}

		inline auto GetCullMode() const
		{
			return mCullMode;
		}

		inline auto GetBlend() const
		{
			return mBlend;
		}

		inline const auto &GetBlendFuncs() const
		{
			return mBlendFuncs;
		}

		inline const auto &GetBlendFactors() const
		{
			return mBlendFactors;
		}

	protected:
		const std::string mName;
		const Shader *mpShader;
		MaterialConstants mMaterialConstants{};
		const Buffer *mpMaterialConstantsBuffer{nullptr};
		const Texture *mpColorMap{nullptr};
		const Texture *mpBumpMap{nullptr};
		bool mDepthTest;
		bool mDepthWrite;
		CompareOp mDepthFunc;
		bool mScissorTest;
		bool mStencilTest;
		std::array<StencilFaceFunc, (size_t)Face::MAX> mStencilFuncs;
		std::array<StencilFaceOp, (size_t)Face::MAX> mStencilOps;
		uint32_t mStencilWriteMask;
		Face mCullMode;
		bool mBlend;
		BlendFuncs mBlendFuncs;
		BlendFactors mBlendFactors;

		BaseMaterial(const MaterialArgs &rArgs, const Buffer *pMaterialConstantsBuffer) : mName(rArgs.name),
																						  mpShader(rArgs.pShader),
																						  mMaterialConstants(rArgs.materialConstants),
																						  mpMaterialConstantsBuffer(pMaterialConstantsBuffer),
																						  mpColorMap(rArgs.pColorMap),
																						  mpBumpMap(rArgs.pBumpMap),
																						  mDepthTest(rArgs.depthTest),
																						  mDepthWrite(rArgs.depthWrite),
																						  mDepthFunc(rArgs.depthFunc),
																						  mScissorTest(rArgs.scissorTest),
																						  mStencilTest(rArgs.stencilTest),
																						  mStencilFuncs(rArgs.stencilFuncs),
																						  mStencilOps(rArgs.stencilOps),
																						  mStencilWriteMask(rArgs.stencilWriteMask),
																						  mCullMode(rArgs.cullMode),
																						  mBlend(rArgs.blend),
																						  mBlendFuncs(rArgs.blendFuncs),
																						  mBlendFactors(rArgs.blendFactors)
		{
			assert(mpShader != nullptr);
			assert(pMaterialConstantsBuffer != nullptr);
		}
		virtual ~BaseMaterial() = default;
	};

}

#endif
