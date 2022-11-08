#ifndef SSAO_APPLICATION_H
#define SSAO_APPLICATION_H

#include <FastCG/Application.h>

#include <vector>
#include <memory.h>

class SSAOApplication : public FastCG::Application
{
public:
	SSAOApplication();

protected:
	void OnStart() override;
};

#endif