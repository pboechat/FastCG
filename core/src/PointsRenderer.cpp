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

#ifdef FIXED_FUNCTION_PIPELINE
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(mpPoints->GetSize());
#endif

	mpPoints->DrawCall();

#ifdef FIXED_FUNCTION_PIPELINE
	glPopAttrib();
#endif
}
