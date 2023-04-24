#ifdef FASTCG_VULKAN

#if defined FASTCG_WINDOWS
#include <FastCG/Platform/Windows/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/Platform/Linux/X11Application.h>
#endif
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>
#include <FastCG/Core/Version.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Assets/AssetSystem.h>

#include <iostream>
#include <cassert>
#include <algorithm>

namespace
{
    template <typename AType>
    struct StrComparer;

    template <>
    struct StrComparer<VkLayerProperties>
    {
        static bool Compare(const VkLayerProperties &a, const char *b)
        {
            return strcmp(a.layerName, b) == 0;
        }
    };

    template <>
    struct StrComparer<VkExtensionProperties>
    {
        static bool Compare(const VkExtensionProperties &a, const char *b)
        {
            return strcmp(a.extensionName, b) == 0;
        }
    };

    template <typename ElementType>
    bool Contains(const std::vector<ElementType> &vector, const char *value)
    {
        return std::find_if(vector.begin(), vector.end(), [&value](const auto &element)
                            { return StrComparer<ElementType>::Compare(element, value); }) != vector.end();
    }

    bool SupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIdx, VkSurfaceKHR surface)
    {
        VkBool32 supportsPresentation;
#if defined FASTCG_WINDOWS
        supportsPresentation = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIdx);
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);
        // Uses visual ID from default visual. Only works because we're using a "simple window".
        auto visualId = XVisualIDFromVisual(DefaultVisual(pDisplay, DefaultScreen(pDisplay)));
        supportsPresentation = vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIdx, display, visualId);
#else
#error "FASTCG: Don't know how to check presentation support"
#endif
        if (!supportsPresentation)
        {
            return false;
        }
        VkBool32 supportsPresentationToSurface;
        FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIdx, surface, &supportsPresentationToSurface));
        return supportsPresentationToSurface;
    }

}

namespace FastCG
{
    VulkanGraphicsSystem::VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs) : BaseGraphicsSystem(rArgs)
    {
    }

    VulkanGraphicsSystem::~VulkanGraphicsSystem() = default;

    void VulkanGraphicsSystem::OnInitialize()
    {
        BaseGraphicsSystem::OnInitialize();

        CreateInstance();
        CreateSurface();
        SelectPhysicalDevice();
        GetPhysicalDeviceMemoryProperties();
        CreateDeviceAndGetQueues();
        CreateSwapChainAndGetImages();
        CreateSynchronizationObjects();
        CreateCommandPoolAndCommandBuffers();
    }

    void VulkanGraphicsSystem::OnFinalize()
    {
        DestroyCommandPoolAndCommandBuffers();
        DestroySynchronizationObjects();
        DestroySwapChainAndClearImages();
        DestroyDeviceAndClearQueues();
        DestroySurface();
        DestroyInstance();

        BaseGraphicsSystem::OnFinalize();
    }

    void VulkanGraphicsSystem::CreateInstance()
    {
        VkApplicationInfo applicationInfo;
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = "";
        // TODO:
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "FastCG";
        applicationInfo.engineVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
        applicationInfo.apiVersion = VK_API_VERSION_1_3;

#if _DEBUG
        uint32_t availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, &availableLayers[0]);

        std::cout << "Available layers:" << std::endl;
        for (const auto &rLayer : availableLayers)
        {
            std::cout << rLayer.layerName << std::endl;
        }
#endif

        std::vector<const char *> usedLayers;
#if _DEBUG
        if (Contains(availableLayers, "VK_LAYER_KHRONOS_validation"))
        {
            usedLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        }
        else
        {
            std::cout << "VK_LAYER_KHRONOS_validation not available, ignoring it" << std::endl;
        }
#endif

        uint32_t availableExtensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, &availableExtensions[0]);

#if _DEBUG
        std::cout << "Available extensions:" << std::endl;
        for (const auto &rExtension : availableExtensions)
        {
            std::cout << rExtension.extensionName << std::endl;
        }
#endif

        std::vector<const char *> extensions;

        if (!Contains(availableExtensions, "VK_KHR_surface"))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't find VK_KHR_surface extension");
        }
        extensions.emplace_back("VK_KHR_surface");

        const char *platformSurfaceExtName =
#if defined FASTCG_WINDOWS
            "VK_KHR_win32_surface"
#elif defined FASTCG_LINUX
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#else
#error "FASTCG: Don't know how to enable surfaces in the current platform"
#endif
            ;
        if (!Contains(availableExtensions, platformSurfaceExtName))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't find platform surface extension");
        }
        extensions.emplace_back(platformSurfaceExtName);

        VkInstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = (uint32_t)usedLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = usedLayers.empty() ? nullptr : &usedLayers[0];
        instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : &extensions[0];

        FASTCG_CHECK_VK_RESULT(vkCreateInstance(&instanceCreateInfo, mAllocationCallbacks.get(), &mInstance));
    }

    void VulkanGraphicsSystem::CreateSurface()
    {
#if defined FASTCG_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.hinstance = WindowsApplication::GetInstance()->GetModule();
        assert(surfaceCreateInfo.hinstance != 0);
        surfaceCreateInfo.hwnd = WindowsApplication::GetInstance()->GetWindow();
        assert(surfaceCreateInfo.hwnd != 0);
        FASTCG_CHECK_VK_RESULT(vkCreateWin32SurfaceKHR(mInstance, &surfaceCreateInfo, mAllocationCallbacks.get(), &mSurface));
#elif defined FASTCG_LINUX
        VkXlibSurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.dpy = X11Application::GetInstance()->GetDisplay();
        assert(surfaceCreateInfo.dpy != nullptr);
        surfaceCreateInfo.window = X11Application::GetInstance()->CreateSimpleWindow();
        FASTCG_CHECK_VK_RESULT(vkCreateXlibSurfaceKHR(mInstance, &surfaceCreateInfo, mAllocationCallbacks.get(), &mSurface));
#else
#error "FASTCG: Don't know how to create presentation surface"
#endif
    }

    void VulkanGraphicsSystem::SelectPhysicalDevice()
    {
        uint32_t numPhysicalDevices;
        FASTCG_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(mInstance, &numPhysicalDevices, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(numPhysicalDevices);
        FASTCG_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(mInstance, &numPhysicalDevices, &physicalDevices[0]));

#if _DEBUG
        std::cout << "Devices:" << std::endl;
        for (auto &rPhysicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(rPhysicalDevice, &properties);
            std::cout << properties.deviceName << std::endl;
        }
#endif

        if (physicalDevices.size() == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: No physical device found");
        }

        std::vector<VkQueueFamilyProperties> queueFamiliesProperties;
        for (auto &rPhysicalDevice : physicalDevices)
        {
            uint32_t queueFamiliesCount;
            vkGetPhysicalDeviceQueueFamilyProperties(rPhysicalDevice, &queueFamiliesCount, nullptr);
            queueFamiliesProperties.resize(queueFamiliesCount);
            vkGetPhysicalDeviceQueueFamilyProperties(rPhysicalDevice, &queueFamiliesCount, &queueFamiliesProperties[0]);

            for (uint32_t queueFamilyIdx = 0; queueFamilyIdx < queueFamiliesCount; ++queueFamilyIdx)
            {
                auto &rQueueFamilyProperties = queueFamiliesProperties[queueFamilyIdx];
                // Not supporting separate graphics and present queues at the moment
                // see: https://github.com/KhronosGroup/Vulkan-Docs/issues/1234
                if ((rQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 &&
                    SupportsPresentation(rPhysicalDevice, queueFamilyIdx, mSurface))
                {
                    mGraphicsAndPresentQueueFamilyIndex = queueFamilyIdx;
                }
                if (mGraphicsAndPresentQueueFamilyIndex != ~0u)
                {
                    mPhysicalDevice = rPhysicalDevice;
                    return;
                }
            }
        }

        FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't find a suitable physical device");
    }

    void VulkanGraphicsSystem::GetPhysicalDeviceMemoryProperties()
    {
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);
    }

    void VulkanGraphicsSystem::CreateDeviceAndGetQueues()
    {
        static const float sc_queuePriorities[] = {1.f};

        uint32_t queueCount = 0;
        VkDeviceQueueCreateInfo deviceQueueCreateInfos[2];

        auto &deviceQueueCreateInfo = deviceQueueCreateInfos[queueCount++];
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pNext = nullptr;
        deviceQueueCreateInfo.flags = 0;
        deviceQueueCreateInfo.queueFamilyIndex = mGraphicsAndPresentQueueFamilyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = sc_queuePriorities;

        std::vector<const char *> extensions;

        extensions.push_back("VK_KHR_swapchain");

        VkDeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = nullptr;
        deviceCreateInfo.flags = 0;
        deviceCreateInfo.queueCreateInfoCount = queueCount;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfos[0];
        deviceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : &extensions[0];
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
        deviceCreateInfo.pEnabledFeatures = nullptr;

        FASTCG_CHECK_VK_RESULT(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, mAllocationCallbacks.get(), &mDevice));

        vkGetDeviceQueue(mDevice, mGraphicsAndPresentQueueFamilyIndex, 0, &mGraphicsAndPresentQueue);
    }

    void VulkanGraphicsSystem::CreateSwapChainAndGetImages()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &surfaceCapabilities));

        if (mArgs.rScreenWidth < surfaceCapabilities.minImageExtent.width || mArgs.rScreenWidth > surfaceCapabilities.maxImageExtent.width)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Invalid screen width");
        }

        if (mArgs.rScreenHeight < surfaceCapabilities.minImageExtent.height || mArgs.rScreenHeight > surfaceCapabilities.maxImageExtent.height)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Invalid screen height");
        }

        if ((surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) != 0)
        {
            mPreTransform = VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
        }
        else
        {
            mPreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }

        mMaxSimultaneousFrames = std::max(std::min(mArgs.maxSimultaneousFrames, surfaceCapabilities.maxImageCount), surfaceCapabilities.minImageCount);

        uint32_t surfaceFormatsCount = 0;
        FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatsCount, nullptr));

        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
        FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatsCount, &surfaceFormats[0]));

        mSwapChainSurfaceFormat = surfaceFormats[0];

        {
            uint32_t presentModesCount;
            FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModesCount, nullptr));
            if (presentModesCount == 0)
            {
                FASTCG_THROW_EXCEPTION(Exception, "no present mode");
            }
            std::vector<VkPresentModeKHR> presentModes(presentModesCount);
            FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModesCount, &presentModes[0]));
            auto it = std::find_if(presentModes.begin(), presentModes.end(), [](const auto &presentMode)
                                   { return presentMode == VK_PRESENT_MODE_MAILBOX_KHR; });
            if (it != presentModes.end())
            {
                mPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; // supposedly, > swapchain image count
            }
            else
            {
                mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
            }
        }

        RecreateSwapChainAndGetImages();
    }

    void VulkanGraphicsSystem::RecreateSwapChainAndGetImages()
    {
        DestroySwapChainAndClearImages();

        VkSwapchainCreateInfoKHR swapChainCreateInfo;
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.pNext = nullptr;
        swapChainCreateInfo.flags = 0;
        swapChainCreateInfo.surface = mSurface;
        swapChainCreateInfo.minImageCount = mMaxSimultaneousFrames;
        swapChainCreateInfo.imageFormat = mSwapChainSurfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = mSwapChainSurfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = {mArgs.rScreenWidth, mArgs.rScreenHeight};
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
        swapChainCreateInfo.preTransform = mPreTransform;
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainCreateInfo.presentMode = mPresentMode;
        swapChainCreateInfo.clipped = VK_TRUE;
        swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        FASTCG_CHECK_VK_RESULT(vkCreateSwapchainKHR(mDevice, &swapChainCreateInfo, mAllocationCallbacks.get(), &mSwapChain));

        uint32_t swapChainCount;
        FASTCG_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(mDevice, mSwapChain, &swapChainCount, nullptr));
        mSwapChainImages.resize(swapChainCount);
        FASTCG_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(mDevice, mSwapChain, &swapChainCount, &mSwapChainImages[0]));
    }

    void VulkanGraphicsSystem::CreateSynchronizationObjects()
    {
        mFrameFences.resize(mMaxSimultaneousFrames);
        mAcquireSwapChainImageSemaphores.resize(mMaxSimultaneousFrames);
        mSubmitFinishedSemaphores.resize(mMaxSimultaneousFrames);

        VkFenceCreateInfo fenceInfo;
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;
        for (uint32_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            FASTCG_CHECK_VK_RESULT(vkCreateFence(mDevice, &fenceInfo, mAllocationCallbacks.get(), &mFrameFences[i]));
            FASTCG_CHECK_VK_RESULT(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, mAllocationCallbacks.get(), &mAcquireSwapChainImageSemaphores[i]));
            FASTCG_CHECK_VK_RESULT(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, mAllocationCallbacks.get(), &mSubmitFinishedSemaphores[i]));
        }
    }

    void VulkanGraphicsSystem::CreateCommandPoolAndCommandBuffers()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = mGraphicsAndPresentQueueFamilyIndex;
        FASTCG_CHECK_VK_RESULT(vkCreateCommandPool(mDevice, &commandPoolCreateInfo, mAllocationCallbacks.get(), &mCommandPool));

        VkCommandBufferAllocateInfo commandBufferAllocateInfo;
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.pNext = nullptr;
        commandBufferAllocateInfo.commandPool = mCommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = mMaxSimultaneousFrames;
        mCommandBuffers.resize(mMaxSimultaneousFrames);
        FASTCG_CHECK_VK_RESULT(vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mCommandBuffers[0]));
    }

    void VulkanGraphicsSystem::DestroyCommandPoolAndCommandBuffers()
    {
        if (mCommandPool != VK_NULL_HANDLE)
        {
            if (!mCommandBuffers.empty())
            {
                vkFreeCommandBuffers(mDevice, mCommandPool, (uint32_t)mCommandBuffers.size(), &mCommandBuffers[0]);
                mCommandBuffers.clear();
            }
            vkDestroyCommandPool(mDevice, mCommandPool, mAllocationCallbacks.get());
            mCommandPool = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsSystem::DestroySynchronizationObjects()
    {
        for (uint32_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            vkDestroySemaphore(mDevice, mAcquireSwapChainImageSemaphores[i], mAllocationCallbacks.get());
            vkDestroySemaphore(mDevice, mSubmitFinishedSemaphores[i], mAllocationCallbacks.get());
            vkDestroyFence(mDevice, mFrameFences[i], mAllocationCallbacks.get());
        }
    }

    void VulkanGraphicsSystem::DestroySwapChainAndClearImages()
    {
        if (mSwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mDevice, mSwapChain, mAllocationCallbacks.get());
            mSwapChain = VK_NULL_HANDLE;
        }
        mSwapChainImages.clear();
    }

    void VulkanGraphicsSystem::DestroyDeviceAndClearQueues()
    {
        if (mDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(mDevice, mAllocationCallbacks.get());
            mDevice = VK_NULL_HANDLE;
        }
        mGraphicsAndPresentQueue = VK_NULL_HANDLE;
        mGraphicsAndPresentQueueFamilyIndex = ~0u;
    }

    void VulkanGraphicsSystem::DestroySurface()
    {
        if (mSurface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(mInstance, mSurface, mAllocationCallbacks.get());
            mSurface = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsSystem::DestroyInstance()
    {
        if (mInstance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(mInstance, mAllocationCallbacks.get());
            mInstance = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsSystem::Present()
    {
    }

    double VulkanGraphicsSystem::GetPresentElapsedTime() const
    {
        return 0;
    }

    double VulkanGraphicsSystem::GetGpuElapsedTime() const
    {
        return 0;
    }
}

#endif