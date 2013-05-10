#include <LineRenderer.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

IMPLEMENT_TYPE(LineRenderer, Renderer);

void LineRenderer::OnRender()
{
	if (mLineStripPtr == 0)
	{
		return;
	}

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_COLOR_MATERIAL);
#endif

	mLineStripPtr->DrawCall();

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPopAttrib();
	glPopMatrix();
#endif
}
