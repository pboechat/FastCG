#ifndef POINTS_H_
#define POINTS_H_

#include <glm/glm.hpp>

#include <vector>

class Points
{
public:
	Points(const std::vector<glm::vec3>& rVertices, float size, const glm::vec4& rColor);
	Points(const std::vector<glm::vec3>& rVertices, float size, const std::vector<glm::vec4>& rColors);
	~Points();

	inline float GetSize() const
	{
		return mSize;
	}

	void DrawCall();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec4> mColors;
	float mSize;
#ifdef FIXED_FUNCTION_PIPELINE
	unsigned int mDisplayListId;
#else
	unsigned int mPointsVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mColorsVBOId;
#endif

	void AllocateResources();
	void DeallocateResources();

};

#endif