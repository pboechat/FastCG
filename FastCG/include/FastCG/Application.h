#ifndef FASTCG_APPLICATION_H
#define FASTCG_APPLICATION_H

#include <FastCG/FastCG.h>

#ifdef FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
namespace FastCG
{
    using Application = WindowsApplication;
}
#else
#error "FASTCG: Unhandled platform"
#endif

#endif