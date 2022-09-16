#ifndef FASTCG_RENDERING_STRATEGY_H_
#define FASTCG_RENDERING_STRATEGY_H_

#include <FastCG/Camera.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/PointLight.h>
#include <FastCG/LineRenderer.h>
#include <FastCG/PointsRenderer.h>
#include <FastCG/RenderBatch.h>
#include <FastCG/RenderingStatistics.h>

#include <glm/glm.hpp>

#include <vector>

namespace FastCG
{
	class RenderingStrategy
	{
	public:
		RenderingStrategy(const uint32_t& rScreenWidth,
			const uint32_t& rScreenHeight,
			const std::vector<Light*>& rLights,
			const std::vector<DirectionalLight*>& rDirectionalLights,
			const std::vector<PointLight*>& rPointLights,
			const glm::vec4& rGlobalAmbientLight,
			const std::vector<std::unique_ptr<RenderBatch>>& rRenderBatches,
			const std::vector<LineRenderer*>& rLineRenderers,
			const std::vector<PointsRenderer*>& rPointsRenderer,
			RenderingStatistics& rRenderingStatistics) :
			mrScreenWidth(rScreenWidth),
			mrScreenHeight(rScreenHeight),
			mrLights(rLights),
			mrDirectionalLights(rDirectionalLights),
			mrPointLights(rPointLights),
			mrGlobalAmbientLight(rGlobalAmbientLight),
			mrRenderBatches(rRenderBatches),
			mrLineRenderers(rLineRenderers),
			mrPointsRenderer(rPointsRenderer),
			mrRenderingStatistics(rRenderingStatistics)
		{
		}
		virtual ~RenderingStrategy() = default;

		virtual void Render(const Camera* pCamera) = 0;

	protected:
		const uint32_t& mrScreenWidth;
		const uint32_t& mrScreenHeight;
		const std::vector<Light*>& mrLights;
		const std::vector<DirectionalLight*>& mrDirectionalLights;
		const std::vector<PointLight*>& mrPointLights;
		const glm::vec4& mrGlobalAmbientLight;
		const std::vector<std::unique_ptr<RenderBatch>>& mrRenderBatches;
		const std::vector<LineRenderer*>& mrLineRenderers;
		const std::vector<PointsRenderer*>& mrPointsRenderer;
		RenderingStatistics& mrRenderingStatistics;

	};

}

#endif