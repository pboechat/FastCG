#if (!defined(FORWARDRENDERINGSTRATEGY_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define FORWARDRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>
#include <Shader.h>

class ForwardRenderingStrategy : public RenderingStrategy
{
public:
	ForwardRenderingStrategy(std::vector<Light*>& rLights,
							 std::vector<DirectionalLight*>& rDirectionalLights,
							 std::vector<PointLight*>& rPointLights,
							 glm::vec4& rGlobalAmbientLight,
							 std::vector<RenderBatch*>& rRenderingGroups,
							 std::vector<LineRenderer*>& rLineRenderers,
							 std::vector<PointsRenderer*>& rPointsRenderer,
							 RenderingStatistics& rRenderingStatistics);

	virtual void Render(const Camera* pCamera);

private:
	Shader* mpLineStripShader;
	Shader* mpPointsShader;

};

#endif