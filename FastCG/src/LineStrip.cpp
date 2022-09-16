#include <FastCG/Shader.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/LineStrip.h>
#include <FastCG/Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
	LineStrip::LineStrip(const std::vector<glm::vec3>& rVertices, const glm::vec4& rColor) :
		mVertices(rVertices)
	{
		mLineStripVAOId = 0;
		mVerticesVBOId = 0;
		mColorsVBOId = 0;

		for (size_t i = 0; i < mVertices.size(); i++)
		{
			mColors.emplace_back(rColor);
		}
	}

	LineStrip::LineStrip(const std::vector<glm::vec3>& rVertices, const std::vector<glm::vec4>& rColors) :
		mVertices(rVertices)
	{
		mLineStripVAOId = 0;
		mVerticesVBOId = 0;
		mColorsVBOId = 0;
		mColors = rColors;
	}

	LineStrip::~LineStrip()
	{
		DeallocateResources();
	}

	void LineStrip::AllocateResources()
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
		glGenVertexArrays(1, &mLineStripVAOId);
		glBindVertexArray(mLineStripVAOId);

		glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
		glEnableVertexAttribArray(Shader::COLORS_ATTRIBUTE_INDEX);

		glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
		glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, mColorsVBOId);
		glVertexAttribPointer(Shader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);

		// TODO: check for errors!
	}

	void LineStrip::DeallocateResources()
	{
		glDeleteBuffers(1, &mColorsVBOId);
		glDeleteBuffers(1, &mVerticesVBOId);
		glDeleteBuffers(1, &mLineStripVAOId);
	}

	void LineStrip::Draw()
	{
		if (mLineStripVAOId == 0)
		{
			AllocateResources();
		}

		glBindVertexArray(mLineStripVAOId);
		glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)mVertices.size());
		glBindVertexArray(0);
	}

}

