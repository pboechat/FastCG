#ifndef FASTCG_POINTS_H
#define FASTCG_POINTS_H

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>

namespace FastCG
{
	class Points
	{
	public:
		Points(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const glm::vec4 &rColor);
		Points(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const std::vector<glm::vec4> &rColors);
		~Points();

		inline float GetSize() const
		{
			return mSize;
		}

		void Draw();

	private:
		std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec4> mColors;
		float mSize;
		GLuint mVertexArrayId{~0u};
		GLuint mVerticesBufferId{~0u};
		GLuint mColorsBufferId{~0u};

		void AllocateResources();
		void DeallocateResources();
	};

}

#endif