#include "BumpMapping2Application.h"
#include <cstdlib>

// memory leak detection
//#include <vld.h>

int main(int argc, char** argv)
{
	BumpMapping2Application app;
	app.Run(argc, argv);
	system("pause");
	return 0;
}
