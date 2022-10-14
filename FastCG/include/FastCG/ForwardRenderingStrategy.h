#ifndef FASTCG_FORWARD_RENDERING_STRATEGY_H
#define FASTCG_FORWARD_RENDERING_STRATEGY_H

#include <FastCG/RenderingPathStrategy.h>
#include <FastCG/Shader.h>

namespace FastCG
{
	class ForwardRenderingStrategy : public RenderingPathStrategy
	{
	public:
		ForwardRenderingStrategy(const uint32_t &rScreenWidth,
								 const uint32_t &rScreenHeight,
								 const glm::vec4 &rAmbientLight,
								 const std::vector<DirectionalLight *> &rDirectionalLights,
								 const std::vector<PointLight *> &rPointLights,
								 const std::vector<LineRenderer *> &rLineRenderers,
								 const std::vector<PointsRenderer *> &rPointsRenderer,
								 const std::vector<std::unique_ptr<RenderBatch>> &rRenderBatches,
								 RenderingStatistics &rRenderingStatistics);

		void Render(const Camera *pCamera) override;

	private:
		std::shared_ptr<Shader> mpLineStripShader;
		std::shared_ptr<Shader> mpPointsShader;
	};

}

#endif