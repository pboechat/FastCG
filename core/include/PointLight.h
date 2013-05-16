#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include <Light.h>

COMPONENT(PointLight, Light)
public:
	virtual void OnInstantiate()
	{
		Light::OnInstantiate();

		mConstantAttenuation = 0.0f;
		mLinearAttenuation = 1.0f;
		mQuadraticAttenuation = 0.0f;
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

private:
	float mConstantAttenuation;
	float mLinearAttenuation;
	float mQuadraticAttenuation;

};

#endif