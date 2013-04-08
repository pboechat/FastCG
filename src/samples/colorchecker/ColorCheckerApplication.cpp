#include "ColorCheckerApplication.h"
#include "Config.h"
#include "ColorMatchingFunctions.h"

#include <FileReader.h>
#include <StringUtils.h>
#include <Exception.h>

#include <iostream>
#include <vector>

ColorCheckerApplication::ColorCheckerApplication() 
	: OpenGLApplication("Color Checker", 800, 600)
{
}

ColorCheckerApplication::~ColorCheckerApplication()
{
	std::map<std::string, LightSpectrum*>::iterator i1 = mBaseColorsLightSpectrums.begin();
	while (i1 != mBaseColorsLightSpectrums.end())
	{
		delete i1->second;
		i1++;
	}

	mBaseColorsLightSpectrums.clear();
	mBaseColorsInRGB.clear();
}

bool ColorCheckerApplication::ParseCommandLineArguments(int argc, char** argv)
{
	if (argc < 2)
	{
		return false;
	}

	mColorCheckerFilename = argv[1];

	return true;
}

void ColorCheckerApplication::PrintUsage()
{
	std::cout << "ColorChecker.exe <color checker filename>" << std::endl;
}

void ColorCheckerApplication::ParseColorCheckerFile()
{
	std::string content = FileReader::Read(Config::DATA_FOLDER_PATH + "/" + mColorCheckerFilename, FM_TEXT);

	std::vector<std::string> lines;
	StringUtils::Tokenize(content, "\n", lines);

	if (lines.size() < 2)
	{
		THROW_EXCEPTION(Exception, "Invalid no. of lines: %d", lines.size());
	}

	// TODO:
	unsigned int start = 380;
	unsigned int end = 730;
	unsigned int precision = 10;

	for (unsigned int i = 1; i < lines.size(); i++)
	{
		std::string line = lines[i];

		std::vector<std::string> elements;
		StringUtils::Tokenize(line, "\t", elements);

		if (elements.size() != 38)
		{
			THROW_EXCEPTION(Exception, "Invalid no. of elements: %d", elements.size());
		}

		float pSamples[35];

		for (unsigned int j = 0; j < 35; j++)
		{
			pSamples[j] = (float)atof(elements[j + 2].c_str());
		}

		LightSpectrum* pLightSpectrum = new LightSpectrum(precision, start, end, pSamples);

		mBaseColorsLightSpectrums.insert(std::make_pair(elements[1], pLightSpectrum));
	}

	// DEBUG:
	/*std::map<std::string, LightSpectrum*>::iterator i = mBaseColorsLightSpectrums.begin();
	while (i != mBaseColorsLightSpectrums.end())
	{
		std::cout << i->first << "\t\t" << i->second->ToString() << std::endl;
		i++;
	}*/
}

void ColorCheckerApplication::ConvertLightSpectrumsToRGBs()
{
	std::map<std::string, LightSpectrum*>::iterator i = mBaseColorsLightSpectrums.begin();
	while (i != mBaseColorsLightSpectrums.end())
	{
		std::string baseColorName = i->first;

		CIEXYZColor cieXYZColor = ColorMatchingFunctions::Apply(*i->second);
		mBaseColorsInRGB.insert(std::make_pair(baseColorName, cieXYZColor.To_sRGB()));

		i++;
	}

	// DEBUG:
	std::map<std::string, sRGBColor>::iterator i2 = mBaseColorsInRGB.begin();
	while (i2 != mBaseColorsInRGB.end())
	{
		std::cout << i2->first << "=" << i2->second.ToString() << std::endl;
		i2++;
	}
}

bool ColorCheckerApplication::OnStart()
{
	try
	{
		ParseColorCheckerFile();
		ConvertLightSpectrumsToRGBs();
	} 
	catch (Exception& e) 
	{
		std::cout << "Error: " << e.GetFullDescription() << std::endl;
		return false;
	}

	return true;
}

void ColorCheckerApplication::OnDisplay()
{
}