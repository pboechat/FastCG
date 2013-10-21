#include "DeferredRenderingApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	DeferredRenderingApplication app;
	return app.Run(argc, argv);
}
