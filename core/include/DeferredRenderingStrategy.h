#if (!defined(DEFERREDRENDERINGSTRATEGY_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define DEFERREDRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>
#include <GBuffer.h>
#include <Shader.h>

class DeferredRenderingStrategy : public RenderingStrategy
{
public:
	DeferredRenderingStrategy(unsigned int& rScreenWidth,
							  unsigned int& rScreenHeight,
							  std::vector<Light*>& rLights,
							  glm::vec4& rGlobalAmbientLight,
							  std::vector<RenderBatch*>& rRenderingGroups,
							  std::vector<LineRenderer*>& rLineRenderers,
							  std::vector<PointsRenderer*>& rPointsRenderer,
							  RenderingStatistics& rRenderingStatistics);
	~DeferredRenderingStrategy();

	virtual void Render(const Camera* pCamera);

private:
	GBuffer* mpGBuffer;
	Shader* mpGeometryPassShader;
	Shader* mpLineStripShader;
	Shader* mpPointsShader;

	void RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection);

};

#endif