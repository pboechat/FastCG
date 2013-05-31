#include "SSAOApplication.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	SSAOApplication app;
	app.Run(argc, argv);
	system("pause");
	return 0;
}
