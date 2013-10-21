#include "SSAOApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	SSAOApplication app;
	return app.Run(argc, argv);
}
