#if (!defined(FIXEDFUNCTIONRENDERINGSTRATEGY_H_) && defined(FIXED_FUNCTION_PIPELINE))
#define FIXEDFUNCTIONRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>

class FixedFunctionRenderingStrategy : public RenderingStrategy
{
public:
	FixedFunctionRenderingStrategy(std::vector<Light*>& rLights,
								   std::vector<DirectionalLight*>& rDirectionalLights,
								   std::vector<PointLight*>& rPointLights,
								   glm::vec4& rGlobalAmbientLight,
								   std::vector<RenderBatch*>& rRenderBatches,
								   std::vector<LineRenderer*>& rLineRenderers,
								   std::vector<PointsRenderer*>& rPointsRenderer,
								   RenderingStatistics& rRenderingStatistics);

	virtual void Render(const Camera* pCamera);

};

#endif