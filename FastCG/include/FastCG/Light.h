#ifndef FASTCG_LIGHT_H
#define FASTCG_LIGHT_H

#include <FastCG/Component.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

namespace FastCG
{
	class Light : public Component
	{
		DECLARE_ABSTRACT_COMPONENT(Light, Component);

	public:
		inline float GetIntensity() const
		{
			return mIntensity;
		}

		inline void SetIntensity(float intensity)
		{
			mIntensity = intensity;
		}

		inline const glm::vec4 &GetDiffuseColor() const
		{
			return mDiffuseColor;
		}

		inline void SetDiffuseColor(const glm::vec4 &color)
		{
			mDiffuseColor = color;
		}

		inline const glm::vec4 &GetSpecularColor() const
		{
			return mSpecularColor;
		}

		inline void SetSpecularColor(const glm::vec4 &color)
		{
			mSpecularColor = color;
		}

	protected:
		glm::vec4 mDiffuseColor{Colors::WHITE};
		glm::vec4 mSpecularColor{Colors::WHITE};
		float mIntensity{1};
	};

}

#endif
