#ifndef DEFERRED_RENDERING_APPLICATION_H
#define DEFERRED_RENDERING_APPLICATION_H

#include <FastCG/Platform/Application.h>

class DeferredRenderingApplication : public FastCG::Application
{
public:
    DeferredRenderingApplication();

protected:
    void OnStart() override;
};

#endif
