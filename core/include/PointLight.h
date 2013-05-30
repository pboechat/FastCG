#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include <Light.h>

COMPONENT(PointLight, Light)
public:
	virtual void OnInstantiate()
	{
		Light::OnInstantiate();

		mConstantAttenuation = 0.0f;
		mLinearAttenuation = 0.0f;
		mQuadraticAttenuation = 1.0f;
	}

	inline float GetConstantAttenuation() const
	{
		return mConstantAttenuation;
	}

	inline void SetConstantAttenuation(float constantAttenuation)
	{
		mConstantAttenuation = constantAttenuation;
	}

	inline float GetLinearAttenuation() const
	{
		return mLinearAttenuation;
	}

	inline void SetLinearAttenuation(float linearAttenuation)
	{
		mLinearAttenuation = linearAttenuation;
	}

	inline float GetQuadraticAttenuation() const
	{
		return mQuadraticAttenuation;
	}

	inline void SetQuadraticAttenuation(float quadraticAttenuation)
	{
		mQuadraticAttenuation = quadraticAttenuation;
	}

private:
	float mConstantAttenuation;
	float mLinearAttenuation;
	float mQuadraticAttenuation;

};

#endif