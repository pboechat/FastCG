#ifndef LIGHTSPECTRUM_H
#define LIGHTSPECTRUM_H

#include <memory>
#include <sstream>
#include <Exception.h>

class LightSpectrum
{
public:
	LightSpectrum(unsigned int precision, unsigned int startingWavelength, unsigned int endingWavelength, float* pSamples) 
		: mPrecision(precision), mStartingWavelength(startingWavelength), mEndingWavelength(endingWavelength)
	{
		unsigned int size = GetSize();
		mpSamples = new float[size];
		memcpy(mpSamples, pSamples, sizeof(float) * size);
	}

	~LightSpectrum() 
	{
		if (mpSamples != NULL)
		{
			delete[] mpSamples;
		}
	}

	inline unsigned int GetStartingWavelength() const
	{
		return mStartingWavelength;
	}

	inline unsigned int GetEndingWavelength() const
	{
		return mEndingWavelength;
	}

	inline unsigned int GetPrecision() const
	{
		return mPrecision;
	}

	inline unsigned int GetSize() const
	{
		unsigned int interval = mEndingWavelength - mStartingWavelength;
		return interval / mPrecision;
	}

	float operator [] (int index) const
	{
		return mpSamples[index];
	}

	LightSpectrum operator * (const LightSpectrum& rOther) const
	{
		if (mStartingWavelength != rOther.mStartingWavelength) {
			THROW_EXCEPTION(Exception, "Invalid spectrum sum");
		}

		if (mEndingWavelength != rOther.mEndingWavelength) {
			THROW_EXCEPTION(Exception, "Invalid spectrum sum");
		}

		if (mPrecision != rOther.mPrecision) {
			THROW_EXCEPTION(Exception, "Invalid spectrum sum");
		}

		unsigned int size = GetSize();
		float* pSamples = new float[size];

		for (unsigned int i = 0; i < size; i++)
		{
			pSamples[i] = mpSamples[i] * rOther.mpSamples[i];
		}

		return LightSpectrum(mPrecision, mStartingWavelength, mEndingWavelength, pSamples);
	}

	std::string ToString() const
	{
		std::stringstream str;
		
		for (unsigned int i = 0; i < GetSize(); i++)
		{
			str << mpSamples[i] << " ";
		}

		return str.str();
	}

private:
	unsigned int mPrecision;
	unsigned int mStartingWavelength;
	unsigned int mEndingWavelength;
	float* mpSamples;

};

#endif