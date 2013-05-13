#ifndef COLORCHECKERAPPLICATION_H
#define COLORCHECKERAPPLICATION_H

#include <Application.h>
#include <Material.h>
#include <Mesh.h>
#include <LightSpectrum.h>
#include <sRGBColor.h>

#include <string>
#include <vector>

class ColorCheckerApplication: public Application
{
public:
	ColorCheckerApplication();

protected:
	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void PrintUsage();
	virtual void OnStart();
	virtual void OnEnd();

private:
	static const unsigned int NUM_COLORS;
	static const unsigned int HORIZONTAL_NUMBER_OF_PATCHES;
	static const unsigned int VERTICAL_NUMBER_OF_PATCHES;
	static const unsigned int BORDER;

	std::string mColorCheckerFilename;
	std::vector<LightSpectrum*> mLightSpectrums;
	std::vector<sRGBColor> mBaseColors;
	std::vector<Material*> mMaterials;
	std::vector<Mesh*> mMeshes;

	void ParseColorCheckerFile();
	void ConvertLightSpectrumsToRGBs();
	void AddColorPatch(float x, float y, float width, float height, const sRGBColor& color);

};

#endif
