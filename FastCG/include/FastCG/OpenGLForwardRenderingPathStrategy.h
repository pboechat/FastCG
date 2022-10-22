#ifndef FASTCG_OPENGL_FORWARD_RENDERING_PATH_STRATEGY_H
#define FASTCG_OPENGL_FORWARD_RENDERING_PATH_STRATEGY_H

#ifdef FASTCG_OPENGL

#include <FastCG/ShaderConstants.h>
#include <FastCG/OpenGLRenderingPathStrategy.h>

namespace FastCG
{
	class OpenGLForwardRenderingPathStrategy : public OpenGLRenderingPathStrategy<ForwardRenderingPath::SceneConstants, ForwardRenderingPath::InstanceConstants, ForwardRenderingPath::LightingConstants>
	{
	public:
		OpenGLForwardRenderingPathStrategy(const RenderingPathStrategyArgs &rArgs) : OpenGLRenderingPathStrategy(rArgs) {}

		void Render(const Camera *pMainCamera) override;
	};

}

#endif

#endif