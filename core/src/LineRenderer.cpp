#include <LineRenderer.h>

COMPONENT_IMPLEMENTATION(LineRenderer, Renderer);

void LineRenderer::OnRender()
{
	if (mpLineStrip == 0)
	{
		return;
	}

	mpLineStrip->DrawCall();
}
