#ifndef FASTCG_RENDERING_PATH_STRATEGY_H
#define FASTCG_RENDERING_PATH_STRATEGY_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/PointsRenderer.h>
#include <FastCG/PointLight.h>
#include <FastCG/MeshBatch.h>
#include <FastCG/LineRenderer.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Camera.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace FastCG
{
	struct RenderingPathStrategyArgs
	{
		const uint32_t &rScreenWidth;
		const uint32_t &rScreenHeight;
		const glm::vec4 &rClearColor;
		const glm::vec4 &rAmbientLight;
		const std::vector<DirectionalLight *> &rDirectionalLights;
		const std::vector<PointLight *> &rPointLights;
		const std::vector<LineRenderer *> &rLineRenderers;
		const std::vector<PointsRenderer *> &rPointsRenderers;
		const std::vector<std::unique_ptr<MeshBatch>> &rMeshBatches;
		RenderingStatistics &rRenderingStatistics;
	};

	class RenderingPathStrategy
	{
	public:
		RenderingPathStrategy(const RenderingPathStrategyArgs &rArgs) : mArgs(rArgs)
		{
		}
		virtual ~RenderingPathStrategy() = default;

		virtual void OnResourcesLoaded() = 0;
		virtual void Render(const Camera *pMainCamera) = 0;

	protected:
		const RenderingPathStrategyArgs mArgs;
	};

}

#endif