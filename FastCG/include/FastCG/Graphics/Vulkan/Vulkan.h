#ifndef FASTCG_VULKAN_H
#define FASTCG_VULKAN_H

#ifdef FASTCG_VULKAN

#if defined FASTCG_WINDOWS
#define NOMINMAX
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#if defined FASTCG_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined FASTCG_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined FASTCG_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
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

#include <vk_mem_alloc.h>

#if defined VK_VERSION_1_3
#define VK_API_VERSION VK_API_VERSION_1_3;
#elif defined VK_VERSION_1_2
#define VK_API_VERSION VK_API_VERSION_1_2;
#elif defined VK_VERSION_1_1
#define VK_API_VERSION VK_API_VERSION_1_1;
#else
#error "FastCG: Unexpected Vulkan API version"
#endif

#endif

#endif