#ifndef COLORCHECKERAPPLICATION_H
#define COLORCHECKERAPPLICATION_H

#include <OpenGLApplication.h>
#include <LightSpectrum.h>
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
	virtual void SetUpViewport();
	virtual void OnDisplay();
	virtual bool OnStart();

private:
	std::string mColorCheckerFilename;
	std::map<std::string, LightSpectrum*> mBaseColorsLightSpectrums;
	std::map<std::string, sRGBColor> mBaseColorsInRGB;
	unsigned int mColorCheckerTextureId;

	void ParseColorCheckerFile();
	void ConvertLightSpectrumsToRGBs();
	void BuildColorCheckerTexture();
	void DrawQuad(unsigned int width, unsigned int height);

};

#endif