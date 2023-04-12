#ifndef FASTCG_LIGHT_H
#define FASTCG_LIGHT_H

#include <FastCG/World/Component.h>
#include <FastCG/Core/Colors.h>

#include <glm/glm.hpp>

namespace FastCG
{
	class Light : public Component
	{
		FASTCG_DECLARE_ABSTRACT_COMPONENT(Light, Component);

	public:
		inline float GetIntensity() const
		{
			return mIntensity;
		}

		inline void SetIntensity(float intensity)
		{
			assert(intensity >= 0);
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

		void OnRegisterInspectableProperties() override
		{
			RegisterInspectableProperty(this, "Diffuse Color", &Light::GetDiffuseColor, &Light::SetDiffuseColor, glm::vec4{0, 0, 0, 0}, glm::vec4{1, 1, 1, 1});
			RegisterInspectableProperty(this, "Specular Color", &Light::GetSpecularColor, &Light::SetSpecularColor, glm::vec4{0, 0, 0, 0}, glm::vec4{1, 1, 1, 1});
			RegisterInspectableProperty(this, "Intensity", &Light::GetIntensity, &Light::SetIntensity, 0.0f, 100.0f);
		}
	};

}

#endif
