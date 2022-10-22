#ifndef FASTCG_OPENGL_DEFERRED_RENDERING_PATH_STRATEGY_H
#define FASTCG_OPENGL_DEFERRED_RENDERING_PATH_STRATEGY_H

#ifdef FASTCG_OPENGL

#include <FastCG/ShaderConstants.h>
#include <FastCG/OpenGLRenderingPathStrategy.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <cstdint>

namespace FastCG
{
	class OpenGLTexture;
	class OpenGLShader;
	class OpenGLMesh;

	class OpenGLDeferredRenderingPathStrategy : public OpenGLRenderingPathStrategy<DeferredRenderingPath::SceneConstants, DeferredRenderingPath::InstanceConstants, DeferredRenderingPath::LightingConstants>
	{
	public:
		OpenGLDeferredRenderingPathStrategy(const RenderingPathStrategyArgs &rArgs);

#ifdef _DEBUG
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
#endif

		inline void SetSSAORadius(float ssaoRadius)
		{
			mSSAOHighFrequencyPassConstants.radius = ssaoRadius;
		}

		inline float GetSSAORadius() const
		{
			return mSSAOHighFrequencyPassConstants.radius;
		}

		inline void SetSSAODistanceScale(float ssaoDistanceScale)
		{
			mSSAOHighFrequencyPassConstants.distanceScale = ssaoDistanceScale;
		}

		inline float GetSSAODistanceScale() const
		{
			return mSSAOHighFrequencyPassConstants.distanceScale;
		}

		inline bool IsSSAOBlurEnabled() const
		{
			return mSSAOBlurEnabled;
		}

		inline void SetSSAOBlurEnabled(bool ssaoBlurEnabled)
		{
			mSSAOBlurEnabled = ssaoBlurEnabled;
		}

		void Initialize() override;
		void PostInitialize() override;
		void Finalize() override;
		void Render(const Camera *pMainCamera) override;

	protected:
		void CreateUniformBuffers() override;
		void DestroyUniformBuffers() override;
		void UpdateSceneConstantsBuffer(const Camera *pMainCamera) override;
		void UpdateSSAOHighFrequencyPassConstantsBuffer();

	private:
		const static uint32_t LIGHT_MESH_DETAIL;
		const static float DEFAULT_SSAO_RADIUS;
		const static float DEFAULT_SSAO_DISTANCE_SCALE;
		const static uint32_t DEFAULT_SSAO_EXPONENT;

		struct LightPassSamplerLocations
		{
			GLint diffuseMap{-1};
			GLint normalMap{-1};
			GLint specularMap{-1};
			GLint tangentMap{-1};
			GLint extraData{-1};
			GLint depth{-1};
			GLint ambientOcclusionMap{-1};
		};

		GLuint mSSAOHighFrequencyPassBufferId{~0u};
		SSAOHighFrequencyPassConstants mSSAOHighFrequencyPassConstants{};
		float mAspectRatio;
		GLuint mGBufferFBOId{~0u};
		GLuint mSSAOFBOId{~0u};
		GLuint mSSAOBlurFBOId{~0u};
		GLuint mNormalTextureId{~0u};
		GLuint mDiffuseTextureId{~0u};
		GLuint mSpecularTextureId{~0u};
		GLuint mTangentTextureId{~0u};
		GLuint mExtraDataTextureId{~0u};
		GLuint mDepthTextureId{~0u};
		GLuint mFinalTextureId{~0u};
		GLuint mSSAOTextureId{~0u};
		GLuint mSSAOBlurTextureId{~0u};
		LightPassSamplerLocations mPointLightPassSamplerLocations;
		LightPassSamplerLocations mDirectionalLightPassSamplerLocations;
		GLint mSSAOHighFrequencyPassNoiseMapLocation{-1};
		GLint mSSAOHighFrequencyPassNormalMapLocation{-1};
		GLint mSSAOHighFrequencyPassDepthLocation{-1};
		GLint mSSAOBlurPassAmbientOcclusionMapLocation{-1};
#ifdef _DEBUG
		GLint mpDepthToScreenDepthLocation{-1};
#endif
		const OpenGLShader *mpStencilPassShader{nullptr};
		const OpenGLShader *mpDirectionalLightPassShader{nullptr};
		const OpenGLShader *mpPointLightPassShader{nullptr};
		const OpenGLShader *mpSSAOHighFrequencyPassShader{nullptr};
		const OpenGLShader *mpSSAOBlurPassShader{nullptr};
		const OpenGLShader *mpDepthToScreenShader{nullptr};
		OpenGLTexture *mpNoiseTexture{nullptr};
		OpenGLTexture *mpEmptySSAOTexture{nullptr};
		OpenGLMesh *mpQuadMesh{nullptr};
		OpenGLMesh *mpSphereMesh{nullptr};
#ifdef _DEBUG
		bool mDisplayGBufferEnabled{false};
		bool mDisplaySSAOTextureEnabled{false};
		bool mShowPointLightsEnabled{false};
#endif
		bool mSSAOBlurEnabled{true};

		void CreateAuxiliaryMeshes();
		void CreateAuxiliaryTextures();
		void AllocateRenderTargetsAndFBOs();
		void FindShaders();
	};

}

#endif

#endif