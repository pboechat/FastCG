#include "ColorCheckerApplication.h"

#include <ColorMatchingFunctions.h>
#include <FileReader.h>
#include <StringUtils.h>
#include <Exception.h>
#include <OpenGLExceptions.h>
#include <MathF.h>
#include <Camera.h>
#include <StandardGeometries.h>

#include <GL/freeglut.h>
#include <iostream>
#include <vector>

const unsigned int ColorCheckerApplication::NUM_COLORS = 24;
const unsigned int ColorCheckerApplication::H_TILES = 6;
const unsigned int ColorCheckerApplication::V_TILES = 4;

ColorCheckerApplication::ColorCheckerApplication() :
		Application("Color Checker", 800, 600)
{
	mMainCamera = Camera(0.0f, -1.0f, 1.0f, 0.0f, (float) mScreenHeight, 0.0f, (float) mScreenWidth, PM_ORTHOGRAPHIC);
}

ColorCheckerApplication::~ColorCheckerApplication()
{
	std::vector<LightSpectrum*>::iterator i = mLightSpectrums.begin();
	while (i != mLightSpectrums.end())
	{
		delete *i;
		i++;
	}

	mLightSpectrums.clear();
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
	std::string content = FileReader::Read(mColorCheckerFilename, FM_TEXT);

	std::vector<std::string> lines;
	StringUtils::Tokenize(content, "\n", lines);

	if (lines.size() != NUM_COLORS + 1)
	{
		THROW_EXCEPTION(Exception, "No. of lines different from %d: %d", NUM_COLORS + 1, lines.size());
	}

	std::string line = lines[0];

	std::vector<std::string> elements;
	StringUtils::Tokenize(line, "\t", elements);

	if (elements.size() < 4)
	{
		THROW_EXCEPTION(Exception, "Not enough elements per line: %d", elements.size());
	}

	unsigned int initialWavelength = atoi(elements[2].c_str());
	unsigned int nextWavelength = atoi(elements[3].c_str());
	unsigned int finalWavelength = atoi(elements[elements.size() - 1].c_str());
	unsigned int precision = (nextWavelength - initialWavelength);

	for (unsigned int i = 1; i < lines.size(); i++)
	{
		line = lines[i];

		elements.clear();
		StringUtils::Tokenize(line, "\t", elements);

		unsigned int numFunctionValues = elements.size() - 3;

		float* pFunctionValues = new float[numFunctionValues];

		for (unsigned int j = 0, k = 2; j < numFunctionValues; j++, k++)
		{
			pFunctionValues[j] = (float) atof(elements[k].c_str());
		}

		LightSpectrum* pLightSpectrum = new LightSpectrum(initialWavelength, finalWavelength, precision, pFunctionValues);

		delete[] pFunctionValues;

		mLightSpectrums.push_back(pLightSpectrum);
	}

#ifdef _DEBUG
	std::cout << "*************************************" << std::endl;

	std::vector<LightSpectrum*>::iterator it = mLightSpectrums.begin();
	while (it != mLightSpectrums.end())
	{
		std::cout << (*it)->ToString() << std::endl << std::endl;
		it++;
	}

	std::cout << "*************************************" << std::endl;
#endif
}

void ColorCheckerApplication::ConvertLightSpectrumsToRGBs()
{
	std::vector<LightSpectrum*>::iterator i1 = mLightSpectrums.begin();
	while (i1 != mLightSpectrums.end())
	{
		CIEXYZColor cieXYZColor = ColorMatchingFunctions::Apply(I_D65, *(*i1));
		mBaseColors.push_back(cieXYZColor.To_sRGB(I_D65, true));

		i1++;
	}

#ifdef _DEBUG
	std::cout << "*************************************" << std::endl;

	std::vector<sRGBColor>::iterator i2 = mBaseColors.begin();
	while (i2 != mBaseColors.end())
	{
		std::cout << i2->ToString() << std::endl << std::endl;
		i2++;
	}

	std::cout << "*************************************" << std::endl;
#endif
}

bool ColorCheckerApplication::OnStart()
{
	try
	{
		ParseColorCheckerFile();
		ConvertLightSpectrumsToRGBs();

		CHECK_FOR_OPENGL_ERRORS()
		;
	} catch (Exception& e)
	{
		std::cout << "Error: " << e.GetFullDescription() << std::endl;
		return false;
	}

	mSolidColorShaderPtr = new Shader("SolidColor");
	mSolidColorShaderPtr->Compile("shaders/SolidColor.vert", ST_VERTEX);
	mSolidColorShaderPtr->Compile("shaders/SolidColor.frag", ST_FRAGMENT);
	mSolidColorShaderPtr->Link();

	float tileWidth = mScreenWidth / (float) H_TILES;
	float tileHeight = mScreenHeight / (float) V_TILES;

	std::vector<sRGBColor>::iterator colorIterator = mBaseColors.begin();
	for (unsigned int y = V_TILES; y > 0; y--)
	{
		for (unsigned int x = 0; x < H_TILES; x++)
		{
			AddColorChecker(x * tileWidth, (y - 1) * tileHeight, tileWidth, tileHeight, *colorIterator);
			colorIterator++;
		}
	}

	return true;
}

void ColorCheckerApplication::AddColorChecker(float x, float y, float width, float height, const sRGBColor& color)
{
	MaterialPtr solidColorMaterialPtr = new Material(mSolidColorShaderPtr);
	solidColorMaterialPtr->SetColor("solidColor", glm::vec4(color.R(), color.G(), color.B(), 1.0f));

	GeometryPtr colorCheckerPtr = StandardGeometries::CreateXYPlane(width, height, 1, 1, glm::vec3(-width * 0.5f, height * 0.5f, 0.0f), solidColorMaterialPtr);
	colorCheckerPtr->Translate(glm::vec3(x, y, 0.0f));
	colorCheckerPtr->SetMaterial(solidColorMaterialPtr);

	AddGeometry(colorCheckerPtr);
}

