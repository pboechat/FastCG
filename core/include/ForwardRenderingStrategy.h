#if (!defined(FORWARDRENDERINGSTRATEGY_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define FORWARDRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>
#include <Shader.h>

class ForwardRenderingStrategy : public RenderingStrategy
{
public:
	ForwardRenderingStrategy(std::vector<Light*>& rLights,
		glm::vec4& rGlobalAmbientLight,
		std::vector<RenderingGroup*>& rRenderingGroups,
		std::vector<LineRenderer*>& rLineRenderers,
		std::vector<PointsRenderer*>& rPointsRenderer);

	virtual void Render(const Camera* pCamera);

private:
	Shader* mpLineStripShader;
	Shader* mpPointsShader;

	void SetUpShaderLights(Shader* pShader);

};

#endif