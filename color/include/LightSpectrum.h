#ifndef LIGHTSPECTRUM_H_
#define LIGHTSPECTRUM_H_

#include <cstring>
#include <sstream>
#include <Exception.h>

class LightSpectrum
{
public:
	LightSpectrum(unsigned int initialWavelength, unsigned int finalWavelength, unsigned int precision, const float* pFunctionValues) :
		mInitialWavelength(initialWavelength), mFinalWavelength(finalWavelength), mPrecision(precision), mpFunctionValues(NULL)
	{
		unsigned int size = GetSize();
		mpFunctionValues = new float[size];
		memcpy(mpFunctionValues, pFunctionValues, sizeof(float) * size);
	}

	~LightSpectrum()
	{
		if (mpFunctionValues != NULL)
		{
			delete[] mpFunctionValues;
		}
	}

	inline unsigned int GetInitialWavelength() const
	{
		return mInitialWavelength;
	}

	inline unsigned int GetFinalWavelength() const
	{
		return mFinalWavelength;
	}

	inline unsigned int GetPrecision() const
	{
		return mPrecision;
	}

	inline unsigned int GetSize() const
	{
		return (mFinalWavelength - mInitialWavelength) / mPrecision;
	}

	inline float operator [](int index) const
	{
		return mpFunctionValues[index];
	}

	std::string ToString() const;

private:
	unsigned int mInitialWavelength;
	unsigned int mFinalWavelength;
	unsigned int mPrecision;
	float* mpFunctionValues;

};

#endif
