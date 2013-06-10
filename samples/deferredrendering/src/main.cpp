#include "DeferredRenderingApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	DeferredRenderingApplication app;
	app.Run(argc, argv);
	system("pause");
	return 0;
}
