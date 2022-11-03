#ifndef FASTCG_BASE_WORLD_RENDERER_H
#define FASTCG_BASE_WORLD_RENDERER_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderBatch.h>
#include <FastCG/PointLight.h>
#include <FastCG/IWorldRenderer.h>
#include <FastCG/DirectionalLight.h>

#include <glm/glm.hpp>

#include <vector>

namespace FastCG
{
	struct WorldRendererArgs
	{
		const uint32_t &rScreenWidth;
		const uint32_t &rScreenHeight;
		const glm::vec4 &rClearColor;
		const glm::vec4 &rAmbientLight;
		const std::vector<DirectionalLight *> &rDirectionalLights;
		const std::vector<PointLight *> &rPointLights;
		const std::vector<const RenderBatch *> &rRenderBatches;
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

		inline void Initialize() override;
		inline void Finalize() override;

	protected:
		const WorldRendererArgs mArgs;
		Buffer *mpInstanceConstantsBuffer{nullptr};
		Buffer *mpLightingConstantsBuffer{nullptr};
		Buffer *mpSceneConstantsBuffer{nullptr};
		InstanceConstants mInstanceConstants{};
		LightingConstants mLightingConstants{};
		SceneConstants mSceneConstants{};

		inline void SetupMaterial(const Material *pMaterial, RenderingContext *pRenderingContext);
		inline void UpdateInstanceConstantsBuffer(const glm::mat4 &model, RenderingContext *pRenderingContext);
		inline void UpdateLightingConstantsBuffer(const PointLight *pPointLight, RenderingContext *pRenderingContext);
		inline void UpdateLightingConstantsBuffer(const DirectionalLight *pDirectionalLight, const glm::vec3 &direction, RenderingContext *pRenderingContext);
		inline virtual void UpdateSceneConstantsBuffer(const Camera *pCamera, RenderingContext *pRenderingContext);
	};

}

#include <FastCG/BaseWorldRenderer.inc>

#endif