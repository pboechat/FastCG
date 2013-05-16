#ifndef RENDERINGSTRATEGY_H_
#define RENDERINGSTRATEGY_H_

#include <Camera.h>
#include <DirectionalLight.h>
#include <PointLight.h>
#include <LineRenderer.h>
#include <PointsRenderer.h>
#include <RenderBatch.h>
#include <RenderingStatistics.h>

#include <glm/glm.hpp>

#include <vector>

class RenderingStrategy
{
public:
	RenderingStrategy(std::vector<Light*>& rLights,
					  std::vector<DirectionalLight*>& rDirectionalLights,
					  std::vector<PointLight*>& rPointLights,
					  glm::vec4& rGlobalAmbientLight,
					  std::vector<RenderBatch*>& rRenderBatches,
					  std::vector<LineRenderer*>& rLineRenderers,
					  std::vector<PointsRenderer*>& rPointsRenderer,
					  RenderingStatistics& rRenderingStatistics) :
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

	virtual ~RenderingStrategy()
	{
	}

	virtual void Render(const Camera* pCamera) = 0;

protected:
	std::vector<Light*>& mrLights;
	std::vector<DirectionalLight*>& mrDirectionalLights;
	std::vector<PointLight*>& mrPointLights;
	glm::vec4& mrGlobalAmbientLight;
	std::vector<RenderBatch*>& mrRenderBatches;
	std::vector<LineRenderer*>& mrLineRenderers;
	std::vector<PointsRenderer*>& mrPointsRenderer;
	RenderingStatistics& mrRenderingStatistics;

};

#endif