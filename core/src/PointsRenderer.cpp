#include <PointsRenderer.h>

COMPONENT_IMPLEMENTATION(PointsRenderer, Renderer);

void PointsRenderer::OnRender()
{
	if (mpPoints == 0)
	{
		return;
	}

	mpPoints->DrawCall();
}
