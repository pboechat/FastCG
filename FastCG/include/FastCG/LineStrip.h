#ifndef FASTCG_LINE_STRIP_H
#define FASTCG_LINE_STRIP_H

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>

namespace FastCG
{
	class LineStrip
	{
	public:
		LineStrip(const std::string &rName, const std::vector<glm::vec3> &rVertices, const glm::vec4 &rColor);
		LineStrip(const std::string &rName, const std::vector<glm::vec3> &rVertices, const std::vector<glm::vec4> &rColors);
		~LineStrip();

		void Draw();

	private:
		std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec4> mColors;
		GLuint mVertexArrayId{~0u};
		GLuint mVerticesBufferId{~0u};
		GLuint mColorsBufferId{~0u};

		void AllocateResources();
		void DeallocateResources();
	};

}

#endif