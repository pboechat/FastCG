#ifndef BSPLINESAPPLICATION_H_
#define BSPLINESAPPLICATION_H_

#include <Application.h>

class BSplinesApplication : public Application
{
public:
	BSplinesApplication();
	virtual ~BSplinesApplication();

	virtual void OnStart();
	virtual void OnMouseButton(int button, int state, int x, int y);

};

#endif