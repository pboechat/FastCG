#include <FastCG/Shader.h>
#include <FastCG/Points.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
	Points::Points(const std::vector<glm::vec3>& rVertices, float size, const glm::vec4& rColor) :
		mVertices(rVertices),
		mSize(size)
	{
		for (size_t i = 0; i < mVertices.size(); i++)
		{
			mColors.emplace_back(rColor);
		}
	}

	Points::Points(const std::vector<glm::vec3>& rVertices, float size, const std::vector<glm::vec4>& rColors) :
		mVertices(rVertices),
		mSize(size),
		mColors(rColors)
	{
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
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);

		// create colors buffer object and attach data
		glGenBuffers(1, &mColorsVBOId);
		glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
		glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(glm::vec4), &mColors[0], GL_STATIC_DRAW);

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
		if (mColorsVBOId != ~0u)
		{
			glDeleteBuffers(1, &mColorsVBOId);
		}
		if (mVerticesVBOId != ~0u)
		{
			glDeleteBuffers(1, &mVerticesVBOId);
		}
		if (mPointsVAOId != ~0u)
		{
			glDeleteBuffers(1, &mPointsVAOId);
		}
	}

	void Points::Draw()
	{
		if (mPointsVAOId == ~0u)
		{
			AllocateResources();
		}

		glBindVertexArray(mPointsVAOId);
		glDrawArrays(GL_POINTS, 0, (GLsizei)mVertices.size());
		glBindVertexArray(0);
	}

}
