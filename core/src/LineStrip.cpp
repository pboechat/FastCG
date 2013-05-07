#include <LineStrip.h>
#include <Exception.h>
#include <ShaderRegistry.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

LineStrip::LineStrip(const std::vector<glm::vec3>& rVertices, const glm::vec4& rColor) :
	mVertices(rVertices),
	mUseSingleColor(true),
	mColor(rColor)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mLineStripVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
#else
	mDisplayListId = 0;
#endif
}

LineStrip::LineStrip(const std::vector<glm::vec3>& rVertices, const std::vector<glm::vec4>& rColors) : 
	mVertices(rVertices), 
	mUseSingleColor(false),
	mColors(rColors)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mLineStripVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
#else
	mDisplayListId = 0;
#endif
}

LineStrip::~LineStrip()
{
	DeallocateResources();
}

void LineStrip::AllocateResources()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (mUseSingleColor)
	{
		mLineMaterialPtr = new Material(ShaderRegistry::Find("SingleColorLine"));
		mLineMaterialPtr->SetVec4("color", mColor);
	}
	else
	{
		mLineMaterialPtr = new Material(ShaderRegistry::Find("MultiColorLine"));
	}

	// create vertex buffer object and attach data
	glGenBuffers(1, &mVerticesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * 3 * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

	if (!mUseSingleColor)
	{
		// create colors buffer object and attach data
		glGenBuffers(1, &mColorsVBOId);
		glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
		glBufferData(GL_ARRAY_BUFFER, mColors.size() * 4 * sizeof(float), &mColors[0], GL_STATIC_DRAW);
	}

	// create vertex array object with all previous vbos attached
	glGenVertexArrays(1, &mLineStripVAOId);
	glBindVertexArray(mLineStripVAOId);

	glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
	if (!mUseSingleColor)
	{
		glEnableVertexAttribArray(Shader::COLORS_ATTRIBUTE_INDEX);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (!mUseSingleColor)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
		glVertexAttribPointer(Shader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// TODO: check for errors!
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	if (!mUseSingleColor)
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}

	// create vertex array buffer and attach data
	glVertexPointer(3, GL_FLOAT, 0, &mVertices[0]);

	if (!mUseSingleColor)
	{
		glColorPointer(4, GL_FLOAT, 0, &mColors[0]);
	}

	mDisplayListId = glGenLists(1);

	CHECK_FOR_OPENGL_ERRORS();

	glNewList(mDisplayListId, GL_COMPILE);

	if (mUseSingleColor)
	{
		glColor4fv(&mColor[0]);
	}

	glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
	glEndList();
#endif
}

void LineStrip::DeallocateResources()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (!mUseSingleColor)
	{
		glDeleteBuffers(1, &mColorsVBOId);
	}
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mLineStripVAOId);
#else
	glDeleteLists(mDisplayListId, 1);
#endif
}

void LineStrip::Draw()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (mLineStripVAOId == 0)
	{
		AllocateResources();
	}

	mLineMaterialPtr->Bind(GetModel());

	glBindVertexArray(mLineStripVAOId);
	glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
	glBindVertexArray(0);

	mLineMaterialPtr->Unbind();
#else
	if (mDisplayListId == 0)
	{
		AllocateResources();
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(&GetModel()[0][0]);

	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_COLOR_MATERIAL);

	glCallList(mDisplayListId);

	glPopAttrib();

	glPopMatrix();
#endif
}

