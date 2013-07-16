#include <PointsRenderer.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

COMPONENT_IMPLEMENTATION(PointsRenderer, Renderer);

void PointsRenderer::OnRender()
{
	if (mpPoints == 0)
	{
		return;
	}

	mpPoints->DrawCall();
}
