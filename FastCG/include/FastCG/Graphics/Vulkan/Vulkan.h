#ifndef FASTCG_VULKAN_H
#define FASTCG_VULKAN_H

#ifdef FASTCG_VULKAN

#if defined FASTCG_WINDOWS
#define NOMINMAX
#include <windows.h>
#elif defined FASTCG_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#if defined FASTCG_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined FASTCG_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#else
#error "FASTCG: Unhandled platform"
#endif

#include <vulkan/vulkan.h>

#if defined FASTCG_WINDOWS
#define NOMINMAX
#include <vulkan/vulkan_win32.h>
#elif defined FASTCG_LINUX
#include <vulkan/vulkan_xlib.h>
#endif

#endif

#endif