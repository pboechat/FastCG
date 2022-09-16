#ifndef FASTCG_LIGHT_H_
#define FASTCG_LIGHT_H_

#include <FastCG/Component.h>
#include <FastCG/Colors.h>

#include <glm/glm.hpp>

namespace FastCG
{
	class Light : public Component
	{
		DECLARE_ABSTRACT_COMPONENT(Light, Component);

	public:
		void OnInstantiate() override
		{
			mAmbientColor = Colors::BLACK;
			mDiffuseColor = Colors::WHITE;
			mSpecularColor = Colors::WHITE;
			mIntensity = 1.0f;
			/*mSpotDirection = glm::vec3(0.0f, 0.0f, -1.0f);
			mSpotCutoff = 15.0f;
			mSpotExponent = 3.0f;*/
		}

		inline float GetIntensity() const
		{
			return mIntensity;
		}

		inline void SetIntensity(float intensity)
		{
			mIntensity = intensity;
		}

		inline const glm::vec4& GetAmbientColor() const
		{
			return mAmbientColor;
		}

		inline void SetAmbientColor(const glm::vec4& color)
		{
			mAmbientColor = color;
		}

		inline const glm::vec4& GetDiffuseColor() const
		{
			return mDiffuseColor;
		}

		inline void SetDiffuseColor(const glm::vec4& color)
		{
			mDiffuseColor = color;
		}

		inline const glm::vec4& GetSpecularColor() const
		{
			return mSpecularColor;
		}

		inline void SetSpecularColor(const glm::vec4& color)
		{
			mSpecularColor = color;
		}

		/*inline glm::vec3 GetSpotDirection() const
		{
			return mSpotDirection;
		}

		inline void SetSpotDirection(glm::vec3 val)
		{
			mSpotDirection = val;
		}

		inline float GetSpotCutoff() const
		{
			return mSpotCutoff;
		}

		inline void SetSpotCutoff(float val)
		{
			mSpotCutoff = val;
		}

		inline float GetSpotExponent() const
		{
			return mSpotExponent;
		}

		inline void SetSpotExponent(float val)
		{
			mSpotExponent = val;
		}*/

	protected:
		glm::vec4 mAmbientColor;
		glm::vec4 mDiffuseColor;
		glm::vec4 mSpecularColor;
		float mIntensity;
		/*glm::vec3 mSpotDirection;
		float mSpotCutoff;
		float mSpotExponent;*/

	};

}

#endif
