#ifndef SSAOAPPLICATION_H_
#define SSAOAPPLICATION_H_

#include <FastCG/Application.h>

using namespace FastCG;

class SSAOApplication : public Application
{
public:
	SSAOApplication();

protected:
	void OnStart() override;

};

#endif