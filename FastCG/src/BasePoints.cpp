#include <FastCG/BasePoints.h>

namespace FastCG
{
	void BasePoints::Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const glm::vec4 &rColor)
	{
		mName = rName;
		mVertices = rVertices;
		mSize = size;
		for (size_t i = 0; i < mVertices.size(); i++)
		{
			mColors.emplace_back(rColor);
		}
	}

	void BasePoints::Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const std::vector<glm::vec4> &rColors)
	{
		mName = rName;
		mVertices = rVertices;
		mSize = size;
		mColors = rColors;
	}

}
