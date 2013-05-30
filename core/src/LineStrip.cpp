#include <LineStrip.h>
#include <Shader.h>
#include <Exception.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

LineStrip::LineStrip(const std::vector<glm::vec3>& rVertices, const glm::vec4& rColor) :
	mVertices(rVertices)
{
#ifdef FIXED_FUNCTION_PIPELINE
	mDisplayListId = 0;
#else
	mLineStripVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
#endif

	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		mColors.push_back(rColor);
	}
}

LineStrip::LineStrip(const std::vector<glm::vec3>& rVertices, const std::vector<glm::vec4>& rColors) : 
	mVertices(rVertices)
{
#ifdef FIXED_FUNCTION_PIPELINE
	mDisplayListId = 0;
#else
	mLineStripVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
#endif

	mColors = rColors;
}

LineStrip::~LineStrip()
{
	DeallocateResources();
}

void LineStrip::AllocateResources()
{
#ifdef FIXED_FUNCTION_PIPELINE
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// create vertex array buffer and attach data
	glVertexPointer(3, GL_FLOAT, 0, &mVertices[0]);

	// create color array buffer and attach data
	glColorPointer(4, GL_FLOAT, 0, &mColors[0]);

	mDisplayListId = glGenLists(1);

	CHECK_FOR_OPENGL_ERRORS();

	glNewList(mDisplayListId, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
	glEndList();
#else
	// create vertex buffer object and attach data
	glGenBuffers(1, &mVerticesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * 3 * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

	// create colors buffer object and attach data
	glGenBuffers(1, &mColorsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mColors.size() * 4 * sizeof(float), &mColors[0], GL_STATIC_DRAW);

	// create vertex array object with all previous vbos attached
	glGenVertexArrays(1, &mLineStripVAOId);
	glBindVertexArray(mLineStripVAOId);

	glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::COLORS_ATTRIBUTE_INDEX);

	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
	glVertexAttribPointer(Shader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// TODO: check for errors!
#endif
}

void LineStrip::DeallocateResources()
{
#ifdef FIXED_FUNCTION_PIPELINE
	glDeleteLists(mDisplayListId, 1);
#else
	glDeleteBuffers(1, &mColorsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mLineStripVAOId);
#endif
}

void LineStrip::DrawCall()
{
#ifdef FIXED_FUNCTION_PIPELINE
	if (mDisplayListId == 0)
	{
		AllocateResources();
	}

	glCallList(mDisplayListId);
#else
	if (mLineStripVAOId == 0)
	{
		AllocateResources();
	}

	glBindVertexArray(mLineStripVAOId);
	glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
	glBindVertexArray(0);
#endif
}
