#include <Points.h>
#include <Shader.h>
#include <Exception.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

Points::Points(const std::vector<glm::vec3>& rVertices, float size, const glm::vec4& rColor) :
	mVertices(rVertices),
	mSize(size)
{
	mPointsVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;

	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		mColors.push_back(rColor);
	}
}

Points::Points(const std::vector<glm::vec3>& rVertices, float size, const std::vector<glm::vec4>& rColors) : 
	mVertices(rVertices), 
	mSize(size)
{
	mPointsVAOId = 0;
	mVerticesVBOId = 0;
	mColorsVBOId = 0;
	mColors = rColors;
}

Points::~Points()
{
	DeallocateResources();
}

void Points::AllocateResources()
{
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
}

void Points::DeallocateResources()
{
	glDeleteBuffers(1, &mColorsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mPointsVAOId);
}

void Points::DrawCall()
{
	if (mPointsVAOId == 0)
	{
		AllocateResources();
	}

	glBindVertexArray(mPointsVAOId);
	glDrawArrays(GL_POINTS, 0, mVertices.size());
	glBindVertexArray(0);
}

