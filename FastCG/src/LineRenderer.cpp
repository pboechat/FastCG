#include <FastCG/LineRenderer.h>

namespace FastCG
{
	IMPLEMENT_COMPONENT(LineRenderer, Renderer);

	void LineRenderer::OnRender()
	{
		if (mpLineStrip == 0)
		{
			return;
		}

		mpLineStrip->Draw();
	}

}