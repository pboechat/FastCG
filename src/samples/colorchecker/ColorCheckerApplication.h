#ifndef COLORCHECKERAPPLICATION_H
#define COLORCHECKERAPPLICATION_H

#include <OpenGLApplication.h>
#include "LightSpectrum.h"
#include <sRGBColor.h>

#include <string>
#include <map>

class ColorCheckerApplication : public OpenGLApplication
{
public:
	ColorCheckerApplication();
	virtual ~ColorCheckerApplication();

protected:
	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void PrintUsage();
	virtual void OnDisplay();
	virtual bool OnStart();

private:
	std::string mColorCheckerFilename;
	std::map<std::string, LightSpectrum*> mBaseColorsLightSpectrums;
	std::map<std::string, sRGBColor> mBaseColorsInRGB;

	void ParseColorCheckerFile();
	void ConvertLightSpectrumsToRGBs();

};

#endif