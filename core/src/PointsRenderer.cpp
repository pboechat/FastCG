#include <PointsRenderer.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

IMPLEMENT_TYPE(PointsRenderer, Renderer);

void PointsRenderer::OnRender()
{
	if (mPointsPtr == 0)
	{
		return;
	}

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_COLOR_MATERIAL);
#endif

	mPointsPtr->DrawCall();

#ifndef USE_PROGRAMMABLE_PIPELINE
	glPopAttrib();
	glPopMatrix();
#endif
}
