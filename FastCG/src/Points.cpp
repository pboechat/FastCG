#include <FastCG/Shader.h>
#include <FastCG/Points.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

namespace FastCG
{
	Points::Points(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const glm::vec4 &rColor) : mName(rName),
																															 mVertices(rVertices),
																															 mSize(size)
	{
		for (size_t i = 0; i < mVertices.size(); i++)
		{
			mColors.emplace_back(rColor);
		}
	}

	Points::Points(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const std::vector<glm::vec4> &rColors) : mName(rName),
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
		// create vertex buffer and upload data
		glGenBuffers(1, &mVerticesBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
#ifdef _DEBUG
		{
			std::string bufferLabel = mName + " Vertices (GL_BUFFER)";
			glObjectLabel(GL_BUFFER, mVerticesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
		}
#endif
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);

		FASTCG_CHECK_OPENGL_ERROR();

		// create colors buffer and upload data
		glGenBuffers(1, &mColorsBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
#ifdef _DEBUG
		{
			std::string bufferLabel = mName + " Colors (GL_BUFFER)";
			glObjectLabel(GL_BUFFER, mVerticesBufferId, (GLsizei)bufferLabel.size(), bufferLabel.c_str());
		}
#endif
		glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(glm::vec4), &mColors[0], GL_STATIC_DRAW);

		FASTCG_CHECK_OPENGL_ERROR();

		// create vertex array with all previous buffers attached
		glGenVertexArrays(1, &mVertexArrayId);
		glBindVertexArray(mVertexArrayId);
#ifdef _DEBUG
		{
			std::string vertexArrayLabel = mName + " (GL_VERTEX_ARRAY)";
			glObjectLabel(GL_VERTEX_ARRAY, mVertexArrayId, (GLsizei)vertexArrayLabel.size(), vertexArrayLabel.c_str());
		}
#endif

		glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
		glEnableVertexAttribArray(Shader::COLORS_ATTRIBUTE_INDEX);

		glBindBuffer(GL_ARRAY_BUFFER, mVerticesBufferId);
		glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, mColorsBufferId);
		glVertexAttribPointer(Shader::COLORS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);

		FASTCG_CHECK_OPENGL_ERROR();
	}

	void Points::DeallocateResources()
	{
		if (mColorsBufferId != ~0u)
		{
			glDeleteBuffers(1, &mColorsBufferId);
		}
		if (mVerticesBufferId != ~0u)
		{
			glDeleteBuffers(1, &mVerticesBufferId);
		}
		if (mVertexArrayId != ~0u)
		{
			glDeleteBuffers(1, &mVertexArrayId);
		}
	}

	void Points::Draw()
	{
		if (mVertexArrayId == ~0u)
		{
			AllocateResources();
		}

		glBindVertexArray(mVertexArrayId);
		glDrawArrays(GL_POINTS, 0, (GLsizei)mVertices.size());
		glBindVertexArray(0);
	}

}
