#include <Points.h>
#include <Exception.h>
#include <ShaderRegistry.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

Points::Points(const std::vector<glm::vec3>& rVertices, float size, const glm::vec4& rColor) :
	mVertices(rVertices),
	mSize(size)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mPointsVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
#else
	mDisplayListId = 0;
#endif

	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		mColors.push_back(rColor);
	}
}

Points::Points(const std::vector<glm::vec3>& rVertices, float size, const std::vector<glm::vec4>& rColors) : 
	mVertices(rVertices), 
	mSize(size)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mPointsVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
#else
	mDisplayListId = 0;
#endif

	mColors = rColors;
}

Points::~Points()
{
	DeallocateResources();
}

void Points::AllocateResources()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mPointsMaterialPtr = new Material(ShaderRegistry::Find("Points"));
	mPointsMaterialPtr->SetFloat("size", mSize);

	// create vertex buffer object and attach data
	glGenBuffers(1, &mVerticesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * 3 * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

	// create colors buffer object and attach data
	glGenBuffers(1, &mColorsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mColors.size() * 4 * sizeof(float), &mColors[0], GL_STATIC_DRAW);

	// create vertex array object with all previous vbos attached
	glGenVertexArrays(1, &mPointsVAOId);
	glBindVertexArray(mPointsVAOId);

	glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::COLORS_ATTRIBUTE_INDEX);

	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
	glVertexAttribPointer(Shader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// TODO: check for errors!
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// create vertex array buffer and attach data
	glVertexPointer(3, GL_FLOAT, 0, &mVertices[0]);

	// create color array buffer and attach data
	glColorPointer(4, GL_FLOAT, 0, &mColors[0]);

	mDisplayListId = glGenLists(1);

	CHECK_FOR_OPENGL_ERRORS();

	glPushAttrib(GL_POINT_BIT);

	glNewList(mDisplayListId, GL_COMPILE);

	glPointSize(mSize);

	glDrawArrays(GL_POINTS, 0, mVertices.size());
	glEndList();

	glPopAttrib();
#endif
}

void Points::DeallocateResources()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	glDeleteBuffers(1, &mColorsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mPointsVAOId);
#else
	glDeleteLists(mDisplayListId, 1);
#endif
}

void Points::Draw()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (mPointsVAOId == 0)
	{
		AllocateResources();
	}

	mPointsMaterialPtr->Bind(GetModel());

	glBindVertexArray(mPointsVAOId);
	glDrawArrays(GL_POINTS, 0, mVertices.size());
	glBindVertexArray(0);

	mPointsMaterialPtr->Unbind();
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

