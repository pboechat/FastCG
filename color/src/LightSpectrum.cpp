#include <LightSpectrum.h>

std::string LightSpectrum::ToString() const
{
	std::stringstream stream;
	stream << "(MinLambda=" << mInitialWavelength << ", MaxLambda=" << mFinalWavelength << ", FunctionValues=";

	for (unsigned int i = 0; i < GetSize(); i++)
	{
		stream << mpFunctionValues[i] << " ";
	}

	stream << ")";
	return stream.str();
}