#ifndef COLORCHECKERAPPLICATION_H
#define COLORCHECKERAPPLICATION_H

#include <Application.h>
#include <Pointer.h>
#include <Shader.h>
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
	virtual void OnStart();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnMouseWheel(int button, int direction, int x, int y);

private:
	static const unsigned int NUM_COLORS;
	static const unsigned int H_NUM_PATCHES;
	static const unsigned int V_NUM_PATCHES;
	static const unsigned int BORDER;

	std::string mColorCheckerFilename;
	std::vector<LightSpectrum*> mLightSpectrums;
	std::vector<sRGBColor> mBaseColors;
#ifdef USE_PROGRAMMABLE_PIPELINE
	ShaderPtr mSolidColorShaderPtr;
#endif

	void ParseColorCheckerFile();
	void ConvertLightSpectrumsToRGBs();
	void AddColorChecker(float x, float y, float width, float height, const sRGBColor& color);
	int GetColorPatchIndex(int x, int y);

};

#endif
