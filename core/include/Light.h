#ifndef LIGHT_H_
#define LIGHT_H_

#include <Transformable.h>
#include <Pointer.h>

#include <glm/glm.hpp>

class Light: public Transformable
{
public:
	Light() :
			mIntensity(1.0f),
			mAmbientColor(1.0f, 1.0f, 1.0f, 1.0f),
			mDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
			mSpecularColor(1.0f, 1.0f, 1.0f, 1.0f)
	{
	}

	virtual ~Light()
	{
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

private:
	float mIntensity;
	glm::vec4 mAmbientColor;
	glm::vec4 mDiffuseColor;
	glm::vec4 mSpecularColor;

};

typedef Pointer<Light> LightPtr;

#endif
