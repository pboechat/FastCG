#include <FastCG/Shader.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/BaseLineStrip.h>

namespace FastCG
{
	void BaseLineStrip::Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, const glm::vec4 &rColor)
	{
		mName = rName;
		mVertices = rVertices;
		for (size_t i = 0; i < mVertices.size(); i++)
		{
			mColors.emplace_back(rColor);
		}
	}

	void BaseLineStrip::Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, const std::vector<glm::vec4> &rColors)
	{
		mName = rName;
		mVertices = rVertices;
		mColors = rColors;
	}
}
