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

#ifdef FIXED_FUNCTION_PIPELINE
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_COLOR_MATERIAL);
#endif

	mpLineStrip->DrawCall();

#ifdef FIXED_FUNCTION_PIPELINE
	glPopAttrib();
#endif
}
