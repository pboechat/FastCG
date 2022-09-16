#ifndef FASTCG_LINE_STRIP_H_
#define FASTCG_LINE_STRIP_H_

#include <glm/glm.hpp>

#include <vector>

namespace FastCG
{
	class LineStrip
	{
	public:
		LineStrip(const std::vector<glm::vec3>& rVertices, const glm::vec4& rColor);
		LineStrip(const std::vector<glm::vec3>& rVertices, const std::vector<glm::vec4>& rColors);
		~LineStrip();

		void Draw();

	private:
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec4> mColors;
		unsigned int mLineStripVAOId;
		unsigned int mVerticesVBOId;
		unsigned int mColorsVBOId;

		void AllocateResources();
		void DeallocateResources();

	};

}

#endif