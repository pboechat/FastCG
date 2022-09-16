#ifndef FASTCG_FORWARD_RENDERING_STRATEGY_H_
#define FASTCG_FORWARD_RENDERING_STRATEGY_H_

#include <FastCG/RenderingStrategy.h>
#include <FastCG/Shader.h>

namespace FastCG
{
	class ForwardRenderingStrategy : public RenderingStrategy
	{
	public:
		ForwardRenderingStrategy(const uint32_t& rScreenWidth,
			const uint32_t& rScreenHeight,
			const std::vector<Light*>& rLights,
			const std::vector<DirectionalLight*>& rDirectionalLights,
			const std::vector<PointLight*>& rPointLights,
			const glm::vec4& rGlobalAmbientLight,
			const std::vector<std::unique_ptr<RenderBatch>>& rRenderingGroups,
			const std::vector<LineRenderer*>& rLineRenderers,
			const std::vector<PointsRenderer*>& rPointsRenderer,
			RenderingStatistics& rRenderingStatistics);

		void Render(const Camera* pCamera) override;

	private:
		std::shared_ptr<Shader> mpLineStripShader;
		std::shared_ptr<Shader> mpPointsShader;

	};

}

#endif