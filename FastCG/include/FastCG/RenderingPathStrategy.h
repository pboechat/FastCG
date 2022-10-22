#ifndef FASTCG_RENDERING_PATH_STRATEGY_H
#define FASTCG_RENDERING_PATH_STRATEGY_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderBatch.h>

#include <glm/glm.hpp>

#include <vector>

namespace FastCG
{
	class DirectionalLight;
	class PointLight;
	class Camera;

	struct RenderingPathStrategyArgs
	{
		const uint32_t &rScreenWidth;
		const uint32_t &rScreenHeight;
		const glm::vec4 &rClearColor;
		const glm::vec4 &rAmbientLight;
		const std::vector<DirectionalLight *> &rDirectionalLights;
		const std::vector<PointLight *> &rPointLights;
		const std::vector<const RenderBatch *> &rRenderBatches;
		RenderingStatistics &rRenderingStatistics;
	};

	class RenderingPathStrategy
	{
	public:
		RenderingPathStrategy(const RenderingPathStrategyArgs &rArgs) : mArgs(rArgs) {}
		virtual ~RenderingPathStrategy() = default;

		virtual void Initialize() {}
		virtual void PostInitialize() {}
		virtual void Finalize() {}
		virtual void Render(const Camera *pMainCamera) = 0;

	protected:
		const RenderingPathStrategyArgs mArgs;
	};

}

#endif