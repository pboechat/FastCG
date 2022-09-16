#ifndef FASTCG_POINTS_H_
#define FASTCG_POINTS_H_

#include <glm/glm.hpp>

#include <vector>

namespace FastCG
{
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

		void Draw();

	private:
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec4> mColors;
		float mSize;
		unsigned int mPointsVAOId{ ~0u };
		unsigned int mVerticesVBOId{ ~0u };
		unsigned int mColorsVBOId{ ~0u };

		void AllocateResources();
		void DeallocateResources();

	};

}

#endif