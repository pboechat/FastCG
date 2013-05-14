#if (!defined(FIXEDFUNCTIONRENDERINGSTRATEGY_H_) && !defined(USE_PROGRAMMABLE_PIPELINE))
#define FIXEDFUNCTIONRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>

class FixedFunctionRenderingStrategy : public RenderingStrategy
{
public:
	FixedFunctionRenderingStrategy(std::vector<Light*>& rLights,
		glm::vec4& rGlobalAmbientLight,
		std::vector<RenderBatch*>& rRenderBatches,
		std::vector<LineRenderer*>& rLineRenderers,
		std::vector<PointsRenderer*>& rPointsRenderer,
		RenderingStatistics& rRenderingStatistics) :
		RenderingStrategy(rLights, rGlobalAmbientLight, rRenderBatches, rLineRenderers, rPointsRenderer, rRenderingStatistics)
	{
	}

	virtual void Render(const Camera* pCamera);

};

#endif