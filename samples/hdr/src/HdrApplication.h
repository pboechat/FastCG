#ifndef HDR_APPLICATION_H
#define HDR_APPLICATION_H

#include <FastCG/Platform/Application.h>

class HdrApplication : public FastCG::Application
{
public:
    HdrApplication();

protected:
    void OnStart() override;
};

#endif
