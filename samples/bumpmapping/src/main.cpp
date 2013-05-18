#include "BumpMappingApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	BumpMappingApplication app;
	app.Run(argc, argv);
	system("pause");
	return 0;
}
