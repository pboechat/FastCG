#ifndef LIGHT_H_
#define LIGHT_H_

#include <Component.h>

#include <glm/glm.hpp>

COMPONENT(Light, Component)
public:
	enum LightType
	{
		LT_DIRECTIONAL,
		LT_POINT,
		LT_SPOT
	};

	virtual void OnInstantiate()
	{
		mLightType = LT_DIRECTIONAL;
		mAmbientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mDiffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mSpecularColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mIntensity = 1.0f;
		mConstantAttenuation = 0.0f;
		mLinearAttenuation = 1.0f;
		mQuadraticAttenuation = 0.0f;
		mSpotDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		mSpotCutoff = 15.0f;
		mSpotExponent = 3.0f;
	}

	inline LightType GetLightType() const
	{
		return mLightType;
	}

	inline void SetLightType(LightType lightType)
	{
		mLightType = lightType;
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

	inline float GetConstantAttenuation() const
	{
		return mConstantAttenuation;
	}

	inline void SetConstantAttenuation(float val)
	{
		mConstantAttenuation = val;
	}

	inline float GetLinearAttenuation() const
	{
		return mLinearAttenuation;
	}

	inline void SetLinearAttenuation(float val)
	{
		mLinearAttenuation = val;
	}

	inline float GetQuadraticAttenuation() const
	{
		return mQuadraticAttenuation;
	}

	inline void SetQuadraticAttenuation(float val)
	{
		mQuadraticAttenuation = val;
	}

	inline glm::vec3 GetSpotDirection() const
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
	}

private:
	LightType mLightType;
	glm::vec4 mAmbientColor;
	glm::vec4 mDiffuseColor;
	glm::vec4 mSpecularColor;
	float mIntensity;
	float mConstantAttenuation;
	float mLinearAttenuation;
	float mQuadraticAttenuation;
	glm::vec3 mSpotDirection;
	float mSpotCutoff;
	float mSpotExponent;

};

#endif
