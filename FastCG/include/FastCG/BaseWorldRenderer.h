#ifndef FASTCG_BASE_WORLD_RENDERER_H
#define FASTCG_BASE_WORLD_RENDERER_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderBatch.h>
#include <FastCG/PointLight.h>
#include <FastCG/Light.h>
#include <FastCG/IWorldRenderer.h>
#include <FastCG/DirectionalLight.h>

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>

namespace FastCG
{
	struct WorldRendererArgs
	{
		const uint32_t &rScreenWidth;
		const uint32_t &rScreenHeight;
		const glm::vec4 &rClearColor;
		const glm::vec4 &rAmbientLight;
		const std::vector<RenderBatch> &rRenderBatches;
		RenderingStatistics &rRenderingStatistics;
	};

	template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
	class BaseWorldRenderer : public IWorldRenderer
	{
	public:
		using SceneConstants = SceneConstantsT;
		using InstanceConstants = InstanceConstantsT;
		using LightingConstants = LightingConstantsT;

		BaseWorldRenderer(const WorldRendererArgs &rArgs) : mArgs(rArgs) {}
		virtual ~BaseWorldRenderer() = default;

		inline float GetShadowMapBias() const override
		{
			return mLightingConstants.pcssData.shadowMapData.bias;
		}

		inline void SetShadowMapBias(float shadowMapBias) override
		{
			mLightingConstants.pcssData.shadowMapData.bias = shadowMapBias;
		}

		inline float GetPCSSUvScale() const override
		{
			return mLightingConstants.pcssData.uvScale;
		}

		inline void SetPCSSUvScale(float uvScale) override
		{
			mLightingConstants.pcssData.uvScale = uvScale;
		}

		inline uint32_t GetPCSSBlockerSearchSamples() const override
		{
			return (uint32_t)mLightingConstants.pcssData.blockerSearchSamples;
		}

		inline void SetPCSSBlockerSearchSamples(uint32_t pcssBlockerSearchSamples) override
		{
			mLightingConstants.pcssData.blockerSearchSamples = pcssBlockerSearchSamples;
		}

		inline uint32_t GetPCSSPCFSamples() const override
		{
			return (uint32_t)mLightingConstants.pcssData.pcfSamples;
		}

		inline void SetPCSSPCFSamples(uint32_t pcssPCFSamples) override
		{
			mLightingConstants.pcssData.pcfSamples = pcssPCFSamples;
		}

		inline float GetSSAORadius() const override
		{
			return mSSAOHighFrequencyPassConstants.radius;
		}

		inline void SetSSAORadius(float radius) override
		{
			mSSAOHighFrequencyPassConstants.radius = radius;
		}

		inline float GetSSAODistanceScale() const override
		{
			return mSSAOHighFrequencyPassConstants.distanceScale;
		}

		inline void SetSSAODistanceScale(float distanceScale) override
		{
			mSSAOHighFrequencyPassConstants.distanceScale = distanceScale;
		}

		inline bool IsSSAOBlurEnabled() const override
		{
			return mSSAOBlurEnabled;
		}

		inline void SetSSAOBlurEnabled(bool ssaoBlurEnabled) override
		{
			mSSAOBlurEnabled = ssaoBlurEnabled;
		}

		inline void Initialize() override;
		inline void Resize() override;
		inline void Finalize() override;

	protected:
		const WorldRendererArgs mArgs;
		Buffer *mpInstanceConstantsBuffer{nullptr};
		Buffer *mpLightingConstantsBuffer{nullptr};
		Buffer *mpSceneConstantsBuffer{nullptr};
		InstanceConstants mInstanceConstants{};
		LightingConstants mLightingConstants{};
		SceneConstants mSceneConstants{};
		const Mesh *mpQuadMesh{nullptr};

		inline void GenerateShadowMaps(RenderingContext *pRenderingContext);
		inline void GenerateAmbientOcculusionMap(const glm::mat4 &rProjection, float fov, const Texture *pDepth, RenderingContext *pRenderingContext);
		inline void Tonemap(const Texture *pSourceRenderTarget, const Texture *pDestinationRenderTarget, RenderingContext *pRenderingContext);
		inline void SetupMaterial(const Material *pMaterial, RenderingContext *pRenderingContext);
		inline void UpdateInstanceConstants(const glm::mat4 &rModel, const glm::mat4 &rView, const glm::mat4 &rProjection, RenderingContext *pRenderingContext);
		inline virtual void UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rInverseView, float nearClip, bool isSSAOEnabled, RenderingContext *pRenderingContext);
		inline virtual void UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rViewDirection, float nearClip, bool isSSAOEnabled, RenderingContext *pRenderingContext);
		inline virtual void UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rInverseView, const glm::mat4 &rProjection, RenderingContext *pRenderingContext);

	private:
		using ShadowMapKey = uint64_t;

		class ShadowMap
		{
		public:
			ShadowMap(const Light *pLight = nullptr, const Texture *pTexture = nullptr) : mpLight(pLight), mpTexture(pTexture)
			{
			}

			inline const Texture *GetTexture() const
			{
				return mpTexture;
			}

			inline glm::mat4 GetView() const
			{
				if (mpLight->GetType().IsDerived(DirectionalLight::TYPE))
				{
					auto *pDirectionalLight = static_cast<const DirectionalLight *>(mpLight);
					auto directionalLightPosition = glm::normalize(pDirectionalLight->GetGameObject()->GetTransform()->GetWorldPosition());
					return glm::lookAt(directionalLightPosition, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});
				}
				else
				{
					return glm::mat4();
				}
			}

			inline glm::mat4 GetProjection() const
			{
				if (mpLight->GetType().IsDerived(DirectionalLight::TYPE))
				{
					constexpr float size = 20;
					constexpr float halfSize = size * 0.5f;
					return glm::ortho(-halfSize, halfSize, -halfSize, halfSize, -size, size);
				}
				else
				{
					return glm::mat4();
				}
			}

		private:
			const Light *mpLight;
			const Texture *mpTexture;
		};

		const Shader *mpShadowMapPassShader{nullptr};
		const Buffer *mpShadowMapPassConstantsBuffer{nullptr};
		ShadowMapPassConstants mShadowMapPassConstants{};
		const Texture *mpEmptyShadowMap{nullptr};
		std::unordered_map<ShadowMapKey, ShadowMap> mShadowMaps;
		std::array<const Texture *, 2> mSSAORenderTargets{};
		const Shader *mpSSAOHighFrequencyPassShader{nullptr};
		const Shader *mpSSAOBlurPassShader{nullptr};
		const Buffer *mpSSAOHighFrequencyPassConstantsBuffer{nullptr};
		SSAOHighFrequencyPassConstants mSSAOHighFrequencyPassConstants{};
		const Texture *mpNoiseTexture{nullptr};
		const Texture *mpEmptySSAOTexture{nullptr};
		bool mSSAOBlurEnabled{true};
		const Shader *mpTonemapShader{nullptr};

		inline void CreateSSAORenderTargets();
		inline void DestroySSAORenderTargets();
		inline ShadowMapKey GetShadowMapKey(const Light *pLight) const;
		inline const ShadowMap &GetOrCreateShadowMap(const Light *pLight);
		inline bool GetShadowMap(const Light *pLight, ShadowMap &rShadowMap) const;
		inline void UpdateShadowMapPassConstants(const glm::mat4 &rModelViewProjection, RenderingContext *pRenderingContext);
		inline void UpdatePCSSConstants(const PointLight *pPointLight, float nearClip, RenderingContext *pRenderingContext);
		inline void UpdatePCSSConstants(const DirectionalLight *pDirectionalLight, float nearClip, RenderingContext *pRenderingContext);
		inline void UpdateSSAOConstants(bool isSSAOEnabled, RenderingContext *pRenderingContext) const;
		inline void UpdateSSAOHighFrequencyPassConstants(const glm::mat4 &rProjection, float fov, const Texture *pDepth, RenderingContext *pRenderingContext);
#ifdef _DEBUG
		inline void DebugMenuItemCallback(int &result);
#endif
	};

}

#include <FastCG/BaseWorldRenderer.inc>

#endif