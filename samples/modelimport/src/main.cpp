#include "ModelImportApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	ModelImportApplication app;
	return app.Run(argc, argv);
}