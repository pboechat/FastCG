#ifndef COLORCHECKERAPPLICATION_H
#define COLORCHECKERAPPLICATION_H

#include <OpenGLApplication.h>

class ColorCheckerApplication : public OpenGLApplication
{
public:
	ColorCheckerApplication();
	virtual ~ColorCheckerApplication();

protected:
	virtual bool ParseCommandLineArguments(int argc, char** argv);
	virtual void PrintUsage();
	//virtual void SetUpViewport();
	//virtual void OnDisplay();
	//virtual void OnResize();
	//virtual bool OnStart();
	//virtual void OnFinish();
	//virtual void OnMouseButton(int button, int state, int x, int y);
	//virtual void OnMouseMove(int x, int y);
	//virtual void OnKeyPress(int key);

};

#endif