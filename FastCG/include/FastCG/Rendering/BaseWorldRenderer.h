#ifndef FASTCG_BASE_WORLD_RENDERER_H
#define FASTCG_BASE_WORLD_RENDERER_H

#include <FastCG/Graphics/GraphicsContextState.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Rendering/Fog.h>
#include <FastCG/Rendering/IWorldRenderer.h>
#include <FastCG/Rendering/Light.h>
#include <FastCG/Rendering/Material.h>
#include <FastCG/Rendering/Mesh.h>
#include <FastCG/Rendering/PointLight.h>
#include <FastCG/Rendering/RenderBatchStrategy.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/RenderingStatistics.h>
#include <FastCG/Rendering/ShaderConstants.h>

#include <glm/glm.hpp>

#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace FastCG
{
    struct WorldRendererArgs
    {
        bool hdr;
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        const glm::vec4 &rClearColor;
        const glm::vec4 &rAmbientLightColor;
        RenderBatchStrategy &rRenderBatchStrategy;
        RenderingStatistics &rRenderingStatistics;
    };

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    class BaseWorldRenderer : public IWorldRenderer
    {
    public:
        using SceneConstants = SceneConstantsT;
        using InstanceConstants = InstanceConstantsT;
        using LightingConstants = LightingConstantsT;

        BaseWorldRenderer(const WorldRendererArgs &rArgs) : mArgs(rArgs)
        {
        }
        virtual ~BaseWorldRenderer() = default;

        inline float GetShadowMapBias() const override
        {
            return mPCSSConstants.pcssData.shadowMapData.bias;
        }

        inline void SetShadowMapBias(float shadowMapBias) override
        {
            mPCSSConstants.pcssData.shadowMapData.bias = shadowMapBias;
        }

        inline float GetPCSSUvScale() const override
        {
            return mPCSSConstants.pcssData.uvScale;
        }

        inline void SetPCSSUvScale(float uvScale) override
        {
            mPCSSConstants.pcssData.uvScale = uvScale;
        }

        inline uint32_t GetPCSSBlockerSearchSamples() const override
        {
            return (uint32_t)mPCSSConstants.pcssData.blockerSearchSamples;
        }

        inline void SetPCSSBlockerSearchSamples(uint32_t pcssBlockerSearchSamples) override
        {
            mPCSSConstants.pcssData.blockerSearchSamples = pcssBlockerSearchSamples;
        }

        inline uint32_t GetPCSSPCFSamples() const override
        {
            return (uint32_t)mPCSSConstants.pcssData.pcfSamples;
        }

        inline void SetPCSSPCFSamples(uint32_t pcssPCFSamples) override
        {
            mPCSSConstants.pcssData.pcfSamples = pcssPCFSamples;
        }

        inline float GetSSAORadius() const override
        {
            return mSSAOHighFrequencyPassConstants.radius;
        }

        inline void SetSSAORadius(float radius) override
        {
            mSSAOHighFrequencyPassConstants.radius = radius;
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
        inline void Render(const Camera *pCamera, GraphicsContext *pGraphicsContext) override;
        inline void Resize() override;
        inline void Finalize() override;

    protected:
        const WorldRendererArgs mArgs;
        std::unique_ptr<Mesh> mpQuadMesh{nullptr};

        virtual void OnRender(const Camera *pCamera, GraphicsContext *pGraphicsContext) = 0;
        inline void GenerateShadowMaps(GraphicsContext *pGraphicsContext);
        inline void GenerateAmbientOcculusionMap(const glm::mat4 &rProjection, float fov, const Texture *pDepth,
                                                 GraphicsContext *pGraphicsContext);
        inline void RenderSkybox(const Texture *pRenderTarget, const Texture *pDepthScencilBuffer,
                                 const Buffer *pSceneConstantsBuffer, const glm::mat4 &rView,
                                 const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext);
        inline void Tonemap(const Texture *pSourceRenderTarget, const Texture *pDestinationRenderTarget,
                            GraphicsContext *pGraphicsContext);
        inline void BindMaterial(const std::shared_ptr<Material> &rpMaterial, GraphicsContext *pGraphicsContext);
        inline void SetGraphicsContextState(const GraphicsContextState &rGraphicsContextState,
                                            GraphicsContext *pGraphicsContext) const;
        inline const Buffer *UpdateInstanceConstants(const glm::mat4 &rModel, const glm::mat4 &rView,
                                                     const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext);
        inline std::pair<uint32_t, const Buffer *> UpdateInstanceConstants(
            const std::vector<const Renderable *> &rRenderables, const glm::mat4 &rView, const glm::mat4 &rProjection,
            GraphicsContext *pGraphicsContext);
        inline virtual const Buffer *EmptyLightingConstants(GraphicsContext *pGraphicsContext);
        inline virtual const Buffer *UpdateLightingConstants(const PointLight *pPointLight,
                                                             const glm::mat4 &rInverseView,
                                                             GraphicsContext *pGraphicsContext);
        inline virtual const Buffer *UpdateLightingConstants(const DirectionalLight *pDirectionalLight,
                                                             const glm::vec3 &rViewDirection,
                                                             GraphicsContext *pGraphicsContext);
        inline const Buffer *EmptyPCSSConstants(GraphicsContext *pGraphicsContext);
        inline const Buffer *UpdatePCSSConstants(const PointLight *pPointLight, float nearClip,
                                                 GraphicsContext *pGraphicsContext);
        inline const Buffer *UpdatePCSSConstants(const DirectionalLight *pDirectionalLight, float nearClip,
                                                 GraphicsContext *pGraphicsContext);
        inline void UpdateSSAOConstants(bool isSSAOEnabled, GraphicsContext *pGraphicsContext) const;
        inline const Buffer *UpdateFogConstants(const Fog *pFog, GraphicsContext *pGraphicsContext);
        inline virtual const Buffer *UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rInverseView,
                                                          const glm::mat4 &rProjection,
                                                          GraphicsContext *pGraphicsContext);

    private:
        using ShadowMapKey = uint64_t;

        class ShadowMap
        {
        public:
            ShadowMap(const Light *pLight = nullptr, const Texture *pTexture = nullptr)
                : mpLight(pLight), mpTexture(pTexture)
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
                    auto directionalLightPosition =
                        glm::normalize(pDirectionalLight->GetGameObject()->GetTransform()->GetWorldPosition());
                    return glm::lookAt(directionalLightPosition, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});
                }
                else
                {
                    return glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
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
                    return glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
                }
            }

        private:
            const Light *mpLight;
            const Texture *mpTexture;
        };

        std::vector<const Buffer *> mInstanceConstantsBuffers;
        size_t mLastInstanceConstantsBufferIdx{0};
        std::vector<const Buffer *> mLightingConstantsBuffers;
        size_t mLastLightingConstantsBufferIdx{0};
        std::vector<const Buffer *> mPCSSConstantsBuffers;
        size_t mLastPCSSConstantsBufferIdx{0};
        std::vector<const Buffer *> mFogConstantsBuffers;
        size_t mLastFogConstantsBufferIdx{0};
        std::vector<const Buffer *> mSceneConstantsBuffers;
        size_t mLastSceneConstantsBufferIdx{0};
        InstanceConstants mInstanceConstants{};
        LightingConstants mLightingConstants{};
        PCSSConstants mPCSSConstants{};
        FogConstants mFogConstants{};
        SceneConstants mSceneConstants{};
        const Shader *mpShadowMapPassShader{nullptr};
        std::vector<const Buffer *> mShadowMapPassConstantsBuffers;
        size_t mLastShadowMapPassConstantsBufferIdx{0};
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

        inline const Buffer *GetShadowMapPassConstantsBuffer();
        inline const Buffer *GetInstanceConstantsBuffer();
        inline const Buffer *GetLightingConstantsBuffer();
        inline const Buffer *GetPCSSConstantsBuffer();
        inline const Buffer *GetFogConstantsBuffer();
        inline const Buffer *GetSceneConstantsBuffer();
        inline void CreateSSAORenderTargets();
        inline void DestroySSAORenderTargets();
        inline ShadowMapKey GetShadowMapKey(const Light *pLight) const;
        inline const ShadowMap &GetOrCreateShadowMap(const Light *pLight);
        inline bool GetShadowMap(const Light *pLight, ShadowMap &rShadowMap) const;
        inline std::pair<uint32_t, const Buffer *> UpdateShadowMapPassConstants(
            const std::vector<const Renderable *> &rRenderables, const glm::mat4 &rView, const glm::mat4 &rProjection,
            GraphicsContext *pGraphicsContext);
        inline void UpdateSSAOHighFrequencyPassConstants(const glm::mat4 &rProjection, float fov, const Texture *pDepth,
                                                         GraphicsContext *pGraphicsContext);
#if _DEBUG
        inline void DebugMenuItemCallback(int &result);
#endif
    };

}

#include <FastCG/Rendering/BaseWorldRenderer.inc>

#endif