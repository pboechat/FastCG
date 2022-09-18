#ifndef FASTCG_RENDERING_PATH_STRATEGY_H_
#define FASTCG_RENDERING_PATH_STRATEGY_H_

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
	class RenderingPathStrategy
	{
	public:
		RenderingPathStrategy(const uint32_t& rScreenWidth,
			const uint32_t& rScreenHeight,
			const glm::vec4& rAmbientLight,
			const std::vector<DirectionalLight*>& rDirectionalLights,
			const std::vector<PointLight*>& rPointLights,
			const std::vector<LineRenderer*>& rLineRenderers,
			const std::vector<PointsRenderer*>& rPointsRenderer,
			const std::vector<std::unique_ptr<RenderBatch>>& rRenderBatches,
			RenderingStatistics& rRenderingStatistics) :
			mrScreenWidth(rScreenWidth),
			mrScreenHeight(rScreenHeight),
			mrAmbientLight(rAmbientLight),
			mrDirectionalLights(rDirectionalLights),
			mrPointLights(rPointLights),
			mrLineRenderers(rLineRenderers),
			mrPointsRenderer(rPointsRenderer),
			mrRenderingStatistics(rRenderingStatistics),
			mrRenderBatches(rRenderBatches)
		{
		}
		virtual ~RenderingPathStrategy() = default;

		virtual void Render(const Camera* pCamera) = 0;

	protected:
		const uint32_t& mrScreenWidth;
		const uint32_t& mrScreenHeight;
		const glm::vec4& mrAmbientLight;
		const std::vector<DirectionalLight*>& mrDirectionalLights;
		const std::vector<PointLight*>& mrPointLights;
		const std::vector<LineRenderer*>& mrLineRenderers;
		const std::vector<PointsRenderer*>& mrPointsRenderer;
		const std::vector<std::unique_ptr<RenderBatch>>& mrRenderBatches;
		RenderingStatistics& mrRenderingStatistics;

	};

}

#endif