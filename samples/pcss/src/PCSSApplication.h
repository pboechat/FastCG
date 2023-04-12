#ifndef PCSS_APPLICATION_H
#define PCSS_APPLICATION_H

#include <FastCG/Platform/Application.h>

#include <vector>
#include <memory.h>

class PCSSApplication : public FastCG::Application
{
public:
    PCSSApplication();

protected:
    void OnStart() override;
};

#endif