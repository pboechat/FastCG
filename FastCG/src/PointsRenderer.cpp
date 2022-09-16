#include <FastCG/PointsRenderer.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(PointsRenderer, Renderer);

	void PointsRenderer::OnRender()
	{
		if (mpPoints == 0)
		{
			return;
		}

		mpPoints->Draw();
	}

}
