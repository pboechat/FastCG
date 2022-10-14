#ifndef FASTCG_BASE_LINE_STRIP_H
#define FASTCG_BASE_LINE_STRIP_H

#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace FastCG
{
	class BaseLineStrip
	{
	public:
		BaseLineStrip() = default;
		virtual ~BaseLineStrip() = default;

		void Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, const glm::vec4 &rColor);
		void Initialize(const std::string &rName, const std::vector<glm::vec3> &rVertices, const std::vector<glm::vec4> &rColors);

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

		virtual void Draw() = 0;

	private:
		std::string mName;
		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec4> mColors;
	};

}

#endif