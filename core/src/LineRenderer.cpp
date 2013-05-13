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

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_COLOR_MATERIAL);
#endif

	mpLineStrip->DrawCall();

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPopAttrib();
#endif
}
