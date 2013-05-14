#ifndef RENDERINGSTRATEGY_H_
#define RENDERINGSTRATEGY_H_

#include <Application.h>
#include <Light.h>
#include <LineRenderer.h>
#include <PointsRenderer.h>
#include <RenderingGroup.h>

#include <glm/glm.hpp>

#include <vector>

class RenderingStrategy
{
public:
	RenderingStrategy(std::vector<Light*>& rLights,
					  glm::vec4& rGlobalAmbientLight,
					  std::vector<RenderingGroup*>& rRenderingGroups,
					  std::vector<LineRenderer*>& rLineRenderers,
					  std::vector<PointsRenderer*>& rPointsRenderer) :
		mrLights(rLights),
		mrGlobalAmbientLight(rGlobalAmbientLight),
		mrRenderingGroups(rRenderingGroups),
		mrLineRenderers(rLineRenderers),
		mrPointsRenderer(rPointsRenderer)
	{
	}

	virtual void Render(const Camera* pCamera) = 0;

protected:
	std::vector<Light*>& mrLights;
	glm::vec4& mrGlobalAmbientLight;
	std::vector<RenderingGroup*>& mrRenderingGroups;
	std::vector<LineRenderer*>& mrLineRenderers;
	std::vector<PointsRenderer*>& mrPointsRenderer;

};

#endif