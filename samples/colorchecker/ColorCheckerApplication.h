#ifndef COLORCHECKERAPPLICATION_H
#define COLORCHECKERAPPLICATION_H

#include <Application.h>
#include <LightSpectrum.h>
#include <sRGBColor.h>

#include <string>
#include <map>

class ColorCheckerApplication: public Application
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
	static const unsigned int NUM_COLORS;
	static const unsigned int H_TILES;
	static const unsigned int V_TILES;

	std::string mColorCheckerFilename;
	std::vector<LightSpectrum*> mLightSpectrums;
	std::vector<sRGBColor> mBaseColors;

	void ParseColorCheckerFile();
	void ConvertLightSpectrumsToRGBs();
	void DrawQuad(float x, float y, float width, float height, const sRGBColor& color);

};

#endif
