#ifndef SSAO_APPLICATION_H
#define SSAO_APPLICATION_H

#include <FastCG/Platform/Application.h>

#include <memory.h>
#include <vector>

class SSAOApplication : public FastCG::Application
{
public:
    SSAOApplication();

protected:
    void OnStart() override;
};

#endif