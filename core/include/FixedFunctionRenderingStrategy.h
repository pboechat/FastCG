#if (!defined(FIXEDFUNCTIONRENDERINGSTRATEGY_H_) && !defined(USE_PROGRAMMABLE_PIPELINE))
#define FIXEDFUNCTIONRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>

class FixedFunctionRenderingStrategy : public RenderingStrategy
{
public:
	FixedFunctionRenderingStrategy(std::vector<Light*>& rLights,
		glm::vec4& rGlobalAmbientLight,
		std::vector<RenderingGroup*>& rRenderingGroups,
		std::vector<LineRenderer*>& rLineRenderers,
		std::vector<PointsRenderer*>& rPointsRenderer) :
		RenderingStrategy(rLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer)
	{
	}

	virtual void Render(const Camera* pCamera);

};

#endif