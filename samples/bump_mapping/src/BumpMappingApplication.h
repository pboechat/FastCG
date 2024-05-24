#ifndef BUMPMAPPING_APPLICATION_H
#define BUMPMAPPING_APPLICATION_H

#include <FastCG/Platform/Application.h>

class BumpMappingApplication : public FastCG::Application
{
public:
    BumpMappingApplication();

protected:
    void OnStart() override;
};

#endif