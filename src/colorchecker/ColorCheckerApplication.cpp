#include "ColorCheckerApplication.h"

ColorCheckerApplication::ColorCheckerApplication() 
	: OpenGLApplication("Color Checker", 800, 600)
{
}

ColorCheckerApplication::~ColorCheckerApplication()
{
}

bool ColorCheckerApplication::ParseCommandLineArguments(int argc, char** argv)
{
	return true;
}

void ColorCheckerApplication::PrintUsage()
{
}