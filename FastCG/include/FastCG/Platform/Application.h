#ifndef FASTCG_APPLICATION_H
#define FASTCG_APPLICATION_H

#if defined FASTCG_WINDOWS
#include <FastCG/Platform/Windows/WindowsApplication.h>
namespace FastCG
{
    using Application = WindowsApplication;
}
#elif defined FASTCG_LINUX
#include <FastCG/Platform/Linux/X11Application.h>
namespace FastCG
{
    using Application = X11Application;
}
#elif defined FASTCG_ANDROID
#include <FastCG/Platform/Android/AndroidApplication.h>
namespace FastCG
{
    using Application = AndroidApplication;
}
#else
#error "FASTCG: Unhandled platform"
#endif

#endif