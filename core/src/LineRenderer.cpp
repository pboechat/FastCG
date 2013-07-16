#include <LineRenderer.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

COMPONENT_IMPLEMENTATION(LineRenderer, Renderer);

void LineRenderer::OnRender()
{
	if (mpLineStrip == 0)
	{
		return;
	}

	mpLineStrip->DrawCall();
}
