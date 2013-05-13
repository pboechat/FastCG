#include "ColorCheckerApplication.h"
#include <cstdlib>

// memory leak checking
//#include <vld.h>

int main(int argc, char** argv)
{
	ColorCheckerApplication app;
	app.Run(argc, argv);
	system("pause");
	return 0;
}
