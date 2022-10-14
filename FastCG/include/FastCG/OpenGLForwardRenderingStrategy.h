#ifndef FASTCG_OPENGL_FORWARD_RENDERING_STRATEGY_H
#define FASTCG_OPENGL_FORWARD_RENDERING_STRATEGY_H

#ifdef FASTCG_OPENGL

#include <FastCG/Shader.h>
#include <FastCG/RenderingPathStrategy.h>

namespace FastCG
{
	class OpenGLForwardRenderingStrategy : public RenderingPathStrategy
	{
	public:
		OpenGLForwardRenderingStrategy(const RenderingPathStrategyArgs &rArgs) : RenderingPathStrategy(rArgs)
		{
		}

		void OnResourcesLoaded() override;
		void Render(const Camera *pMainCamera) override;

	private:
		std::shared_ptr<Shader> mpLineStripShader;
		std::shared_ptr<Shader> mpPointsShader;
	};

}

#endif

#endif