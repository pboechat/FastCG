#include "ModelImportApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	ModelImportApplication app;
	app.Run(argc, argv);
	system("pause");
	return 0;
}