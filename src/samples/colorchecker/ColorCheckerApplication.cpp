#include "ColorCheckerApplication.h"
#include "Config.h"

#include <ColorMatchingFunctions.h>
#include <FileReader.h>
#include <StringUtils.h>
#include <Exception.h>
#include <OpenGLExceptions.h>
#include <MathF.h>

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

	std::cout << "*************************************" << std::endl;

	std::map<std::string, LightSpectrum*>::iterator i = mBaseColorsLightSpectrums.begin();
	while (i != mBaseColorsLightSpectrums.end())
	{
		std::cout << i->first << "=" << i->second->ToString() << std::endl << std::endl;
		i++;
	}

	std::cout << "*************************************" << std::endl;
}

void ColorCheckerApplication::ConvertLightSpectrumsToRGBs()
{
	std::map<std::string, LightSpectrum*>::iterator i1 = mBaseColorsLightSpectrums.begin();
	while (i1 != mBaseColorsLightSpectrums.end())
	{
		std::string baseColorName = i1->first;

		LightSpectrum reflectanceSpectrum = LightSpectrum::D65 * (*i1->second);

		CIEXYZColor cieXYZColor = ColorMatchingFunctions::Apply(LightSpectrum::D65, reflectanceSpectrum);
		mBaseColorsInRGB.insert(std::make_pair(baseColorName, cieXYZColor.To_sRGB(I_D65, true)));

		i1++;
	}

	// DEBUG:

	std::cout << "*************************************" << std::endl;

	std::map<std::string, sRGBColor>::iterator i2 = mBaseColorsInRGB.begin();
	while (i2 != mBaseColorsInRGB.end())
	{
		std::cout << i2->first << "=" << i2->second.ToString() << std::endl << std::endl;
		i2++;
	}

	std::cout << "*************************************" << std::endl;
}

void ColorCheckerApplication::BuildColorCheckerTexture()
{
	float* pData = new float[800 * 600 * 3];

	// TODO:
	unsigned int chartWidth = 6;
	unsigned int chartHeight = 4;

	unsigned int checkerWidth = (800 / chartWidth);
	unsigned int checkerHeight = (600 / chartHeight);

	std::map<std::string, sRGBColor>::iterator colorIterator = mBaseColorsInRGB.begin();
	unsigned int lineWidth = chartWidth * checkerWidth;
	for (unsigned int height = 0; height < chartHeight; height++)
	{
		unsigned int top = (height * checkerHeight) * lineWidth;

		for (unsigned int width = 0; width < chartWidth; width++)
		{
			float r = colorIterator->second.R();
			float g = colorIterator->second.G();
			float b = colorIterator->second.B();

			int left = width * checkerWidth;

			for (unsigned int y = 0; y < checkerHeight; y++) 
			{
				for (unsigned int x = 0; x < checkerWidth; x++) 
				{
					int c = (top + (y * lineWidth) + left + x) * 3;
					pData[c] = r;
					pData[c + 1] = g;
					pData[c + 2] = b;
				}
			}

			colorIterator++;
		}
	}

	mColorCheckerTextureId = AllocateTexture(GL_TEXTURE_2D, 800, 600, GL_RGB, GL_RGB, pData);
}

void ColorCheckerApplication::SetUpViewport()
{
	glMatrixMode(GL_PROJECTION);
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	glLoadMatrixf(&projection[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, 800, 600);
}

void ColorCheckerApplication::DrawQuad(unsigned int width, unsigned int height)
{
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex2d(0, 0);

		glTexCoord2d(1, 0);
		glVertex2d(width, 0);

		glTexCoord2d(1, 1);
		glVertex2d(width, height);

		glTexCoord2d(0, 1);
		glVertex2d(0, height);
	glEnd();
}

bool ColorCheckerApplication::OnStart()
{
	glEnable(GL_TEXTURE_2D);

	try
	{
		ParseColorCheckerFile();
		ConvertLightSpectrumsToRGBs();
		BuildColorCheckerTexture();

		CHECK_FOR_OPENGL_ERRORS();
	} 
	catch (Exception& e) 
	{
		std::cout << "Error: " << e.GetFullDescription() << std::endl;
		return false;
	}

	glClearColor(0, 0, 0, 0);

	return true;
}

void ColorCheckerApplication::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, mColorCheckerTextureId);
	DrawQuad(800, 600);

	glutSwapBuffers();
}