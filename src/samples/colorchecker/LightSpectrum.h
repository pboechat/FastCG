#ifndef LIGHTSPECTRUM_H
#define LIGHTSPECTRUM_H

#include <memory>
#include <sstream>

class LightSpectrum
{
public:
	LightSpectrum(unsigned int precision, unsigned int start, unsigned int end, float* pSamples) 
		: mPrecision(precision), mStart(start), mEnd(end)
	{
		unsigned int interval = mEnd - mStart;
		unsigned int size = interval / mPrecision;
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

	inline unsigned int GetPrecision() const
	{
		return mPrecision;
	}

	float operator [] (int index)
	{
		return mpSamples[index];
	}

	std::string ToString() const
	{
		std::stringstream str;
		unsigned int interval = mEnd - mStart;
		unsigned int size = interval / mPrecision;

		for (unsigned int i = 0; i < size; i++)
		{
			str << mpSamples[i] << " ";
		}

		return str.str();
	}

private:
	unsigned int mPrecision;
	unsigned int mStart;
	unsigned int mEnd;
	float* mpSamples;

};

#endif