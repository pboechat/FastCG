#ifndef LINESTRIP_H_
#define LINESTRIP_H_

#include <glm/glm.hpp>

#include <vector>

class LineStrip
{
public:
	LineStrip(const std::vector<glm::vec3>& rVertices, const glm::vec4& rColor);
	LineStrip(const std::vector<glm::vec3>& rVertices, const std::vector<glm::vec4>& rColors);
	~LineStrip();

	void DrawCall();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec4> mColors;
#ifdef FIXED_FUNCTION_PIPELINE
	unsigned int mDisplayListId;
#else
	unsigned int mLineStripVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mColorsVBOId;
#endif

	void AllocateResources();
	void DeallocateResources();

};

#endif