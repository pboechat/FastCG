#ifndef FASTCG_BASE_POINTS_H
#define FASTCG_BASE_POINTS_H

#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace FastCG
{
	class BasePoints
	{
	public:
		BasePoints() = default;
		virtual ~BasePoints() = default;

		void Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const glm::vec4 &rColor);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, float size, const std::vector<glm::vec4> &rColors);

		inline const std::string &GetName() const
		{
			return mName;
		}

		inline const std::vector<glm::vec3> &GetVertices() const
		{
			return mVertices;
		}

		inline const std::vector<glm::vec4> &GetColors() const
		{
			return mColors;
		}

		inline float GetSize() const
		{
			return mSize;
		}

		virtual void Draw() = 0;

	private:
		std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec4> mColors;
		float mSize;
	};

}

#endif