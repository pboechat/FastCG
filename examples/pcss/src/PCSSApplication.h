#ifndef PCSS_APPLICATION_H
#define PCSS_APPLICATION_H

#include <FastCG/Platform/Application.h>

#include <memory.h>
#include <vector>

class PCSSApplication : public FastCG::Application
{
public:
    PCSSApplication();

protected:
    void OnStart() override;
};

#endif