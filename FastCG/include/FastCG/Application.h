#ifndef FASTCG_APPLICATION_H
#define FASTCG_APPLICATION_H

#if defined FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
namespace FastCG
{
    using Application = WindowsApplication;
}
#elif defined FASTCG_LINUX
#include <FastCG/X11Application.h>
namespace FastCG
{
    using Application = X11Application;
}
#else
#error "FASTCG: Unhandled platform"
#endif

#endif