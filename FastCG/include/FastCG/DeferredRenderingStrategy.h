#ifndef FASTCG_DEFERRED_RENDERING_STRATEGY_H_
#define FASTCG_DEFERRED_RENDERING_STRATEGY_H_

#include <FastCG/RenderingStrategy.h>
#include <FastCG/Texture.h>
#include <FastCG/Shader.h>
#include <FastCG/Mesh.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <cstdint>

namespace FastCG
{
	class DeferredRenderingStrategy : public RenderingStrategy
	{
	public:
		DeferredRenderingStrategy(const uint32_t& rScreenWidth,
			const uint32_t& rScreenHeight,
			const std::vector<Light*>& rLights,
			const std::vector<DirectionalLight*>& rDirectionalLights,
			const std::vector<PointLight*>& rPointLights,
			const glm::vec4& rGlobalAmbientLight,
			const std::vector<std::unique_ptr<RenderBatch>>& rRenderingGroups,
			const std::vector<LineRenderer*>& rLineRenderers,
			const std::vector<PointsRenderer*>& rPointsRenderer,
			RenderingStatistics& rRenderingStatistics);

		virtual ~DeferredRenderingStrategy();

		inline bool IsDisplayGBufferEnabled() const
		{
			return mDisplayGBufferEnabled;
		}

		inline void SetDisplayGBufferEnabled(bool displayGBufferEnabled)
		{
			mDisplayGBufferEnabled = displayGBufferEnabled;
		}

		inline bool IsDisplaySSAOTextureEnabled() const
		{
			return mDisplaySSAOTextureEnabled;
		}

		inline void SetDisplaySSAOTextureEnabled(bool displaySSAOTextureEnabled)
		{
			mDisplaySSAOTextureEnabled = displaySSAOTextureEnabled;
		}

		inline bool IsShowPointLightsEnabled() const
		{
			return mShowPointLightsEnabled;
		}

		inline void SetShowPointLightsEnabled(bool showPointLightsEnabled)
		{
			mShowPointLightsEnabled = showPointLightsEnabled;
		}

		inline void SetSSAORadius(float ssaoRayLength)
		{
			mSSAORadius = ssaoRayLength;
		}

		inline float GetSSAORadius() const
		{
			return mSSAORadius;
		}

		inline void SetSSAODistanceScale(float ssaoDistanceScale)
		{
			mSSAODistanceScale = ssaoDistanceScale;
		}

		inline float GetSSAODistanceScale() const
		{
			return mSSAODistanceScale;
		}

		inline bool IsSSAOBlurEnabled() const
		{
			return mSSAOBlurEnabled;
		}

		inline void SetSSAOBlurEnabled(bool ssaoBlurEnabled)
		{
			mSSAOBlurEnabled = ssaoBlurEnabled;
		}

		virtual void Render(const Camera* pCamera);

	private:
		const static uint32_t NUMBER_OF_RANDOM_SAMPLES;
		const static uint32_t LIGHT_MESH_DETAIL;
		const static float DEFAULT_SSAO_RADIUS;
		const static float DEFAULT_SSAO_DISTANCE_SCALE;
		const static uint32_t DEFAULT_SSAO_EXPONENT;
		const static uint32_t NOISE_TEXTURE_WIDTH;
		const static uint32_t NOISE_TEXTURE_HEIGHT;
		const static uint32_t NOISE_TEXTURE_SIZE;

		uint32_t mHalfScreenWidth;
		uint32_t mHalfScreenHeight;
		float mAspectRatio;
		uint32_t mGBufferFBOId{ ~0u };
		uint32_t mSSAOFBOId{ ~0u };
		uint32_t mSSAOBlurFBOId{ ~0u };
		uint32_t mNormalTextureId{ ~0u };
		uint32_t mDiffuseTextureId{ ~0u };
		uint32_t mSpecularTextureId{ ~0u };
		uint32_t mDepthTextureId{ ~0u };
		uint32_t mSSAOTextureId{ ~0u };
		uint32_t mBlurredSSAOTextureId{ ~0u };
		uint32_t mFinalOutputTextureId{ ~0u };
		std::shared_ptr<Shader> mpStencilPassShader{ nullptr };
		std::shared_ptr<Shader> mpDirectionalLightPassShader{ nullptr };
		std::shared_ptr<Shader> mpPointLightPassShader{ nullptr };
		std::shared_ptr<Shader> mpSSAOHighFrequencyPassShader{ nullptr };
		std::shared_ptr<Shader> mpSSAOBlurPassShader{ nullptr };
		std::shared_ptr<Shader> mpLineStripShader{ nullptr };
		std::shared_ptr<Shader> mpPointsShader{ nullptr };
		std::shared_ptr<Shader> mpDepthToScreenShader{ nullptr };
		std::shared_ptr<Texture> mpNoiseTexture{ nullptr };
		std::unique_ptr<Mesh> mpQuadMesh{ nullptr };
		std::unique_ptr<Mesh> mpSphereMesh{ nullptr };
		bool mDisplayGBufferEnabled{ false };
		bool mDisplaySSAOTextureEnabled{ false };
		bool mShowPointLightsEnabled{ false };
		float mSSAORadius;
		float mSSAODistanceScale;
		bool mSSAOBlurEnabled{ true };
		std::vector<glm::vec3> mRandomSamples;

		void BuildAuxiliaryMeshes();
		void FindShaders();
		void GenerateNoiseTexture();
		void GenerateRandomSamplesInAHemisphere();
		void AllocateTexturesAndFBOs();
		void RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection);
		float CalculateLightBoundingBoxScale(PointLight* pLight);

	};

}

#endif