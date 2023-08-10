#ifdef FASTCG_VULKAN

#if defined FASTCG_WINDOWS
#include <FastCG/Platform/Windows/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/Platform/Linux/X11Application.h>
#endif
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>
#include <FastCG/Core/Version.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Assets/AssetSystem.h>

#include <string>
#include <limits>
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
        auto *pDisplay = FastCG::X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);
        // Uses visual ID from default visual. Only works because we're using a "simple window".
        auto visualId = XVisualIDFromVisual(DefaultVisual(pDisplay, DefaultScreen(pDisplay)));
        supportsPresentation = vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIdx, pDisplay, visualId);
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

#if _DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                              void *pUserData)
    {
        std::cerr << "[VULKAN]"
                  << " - " << FastCG::GetVkDebugUtilsMessageSeverityFlagBitsString(messageSeverity);

        // if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
        // {
        //     FASTCG_BREAK_TO_DEBUGGER();
        // }

        bool prevType = false;
        if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) != 0)
        {
            std::cerr << " - " << FastCG::GetVkDebugUtilsMessageTypeFlagBitsString(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
            prevType = true;
        }
        if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0)
        {
            std::cerr << (prevType ? "|" : " - ") << FastCG::GetVkDebugUtilsMessageTypeFlagBitsString(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
            prevType = true;
        }
        if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0)
        {
            std::cerr << (prevType ? "|" : " - ") << FastCG::GetVkDebugUtilsMessageTypeFlagBitsString(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
            prevType = true;
        }
        std::cerr << " - " << pCallbackData->messageIdNumber
                  << " - " << pCallbackData->pMessage
                  << std::endl;
        return VK_FALSE;
    }
#endif

#define FASTCG_VK_EXT_FN(fn) \
    PFN_##fn fn = nullptr

    namespace VkExt
    {
#if _DEBUG
        FASTCG_VK_EXT_FN(vkCreateDebugUtilsMessengerEXT);
        FASTCG_VK_EXT_FN(vkDestroyDebugUtilsMessengerEXT);
        FASTCG_VK_EXT_FN(vkCmdBeginDebugUtilsLabelEXT);
        FASTCG_VK_EXT_FN(vkCmdEndDebugUtilsLabelEXT);
        FASTCG_VK_EXT_FN(vkSetDebugUtilsObjectNameEXT);
#endif
    }

#undef FASTCG_VK_EXT_FN

#define FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, fn)                                                                 \
    VkExt::fn = (PFN_##fn)vkGetInstanceProcAddr(instance, #fn);                                                      \
    if (VkExt::fn == nullptr)                                                                                        \
    {                                                                                                                \
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: Failed to load Vulkan instance extension function " #fn); \
    }

    void LoadVulkanInstanceExtensionFunctions(VkInstance instance)
    {
#if _DEBUG
        FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkCreateDebugUtilsMessengerEXT);
        FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkDestroyDebugUtilsMessengerEXT);
        FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkCmdBeginDebugUtilsLabelEXT);
        FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkCmdEndDebugUtilsLabelEXT);
        FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkSetDebugUtilsObjectNameEXT);
#endif
    }

#undef FASTCG_LOAD_VK_INSTANCE_EXT_FN

#define FASTCG_LOAD_VK_DEVICE_EXT_FN(instance, fn)                                                                 \
    VkExt::fn = (PFN_##fn)vkGetDeviceProcAddr(instance, #fn);                                                      \
    if (VkExt::fn == nullptr)                                                                                      \
    {                                                                                                              \
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: Failed to load Vulkan device extension function " #fn); \
    }

    void LoadVulkanDeviceExtensionFunctions(VkDevice device)
    {
    }

#undef FASTCG_LOAD_VK_DEVICE_EXT_FN

    struct AttachmentDefinition
    {
        const FastCG::VulkanTexture *pTexture;
        VkAttachmentLoadOp colorOrDepth;
        VkAttachmentLoadOp stencil;
        bool write;
    };

    size_t GetRenderPassHash(const std::vector<AttachmentDefinition> &rAttachmentDefinitions)
    {
        // TODO: use renderpass compatibility rule :
        // https://registry.khronos.org/vulkan/specs/1.1-extensions/html/vkspec.html#renderpass-compatibility
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rAttachmentDefinitions[0]), rAttachmentDefinitions.size() * sizeof(&rAttachmentDefinitions[0]));
    }

    size_t GetPipelineHash(const FastCG::VulkanPipelineDescription &rPipelineDescription)
    {
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rPipelineDescription), sizeof(rPipelineDescription));
    }

    size_t GetPipelineLayoutHash(const FastCG::VulkanPipelineLayoutDescription &rPipelineLayoutDescription)
    {
        assert(!rPipelineLayoutDescription.empty());
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rPipelineLayoutDescription[0]), rPipelineLayoutDescription.size() * sizeof(&rPipelineLayoutDescription[0]));
    }

    size_t GetDescriptorSetLayoutHash(const FastCG::VulkanDescriptorSetLayout &rDescriptorSetLayout)
    {
        assert(!rDescriptorSetLayout.empty());
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rDescriptorSetLayout[0]), rDescriptorSetLayout.size() * sizeof(&rDescriptorSetLayout[0]));
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
        GetPhysicalDeviceProperties();
        CreateDeviceAndGetQueues();
        CreateAllocator();
        CreateSynchronizationObjects();
        CreateCommandPoolAndCommandBuffers();
        CreateDescriptorPool();
        BeginCurrentCommandBuffer();
        CreateImmediateGraphicsContext();
        RecreateSwapChainAndGetImages();
    }

    void VulkanGraphicsSystem::OnPreFinalize()
    {
        BaseGraphicsSystem::OnPreFinalize();

        FASTCG_CHECK_VK_RESULT(vkDeviceWaitIdle(mDevice));
    }

    void VulkanGraphicsSystem::OnPostFinalize()
    {
        FinalizeDeferredDestroys();
        DestroyPipelines();
        DestroyPipelineLayouts();
        DestroyFrameBuffers();
        DestroyRenderPasses();
        DestroyDescriptorSetLayouts();
        DestroySwapChainAndClearImages();
        DestroyDescriptorPool();
        DestroyCommandPoolAndCommandBuffers();
        DestroySynchronizationObjects();
        DestroyAllocator();
        DestroyDeviceAndClearQueues();
        DestroySurface();
        DestroyInstance();

        BaseGraphicsSystem::OnPostFinalize();
    }

    void VulkanGraphicsSystem::CreateInstance()
    {
        VkApplicationInfo applicationInfo;
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = "";
        // TODO: provide a mechanism for uses to specify their app versions
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

#if _DEBUG
        if (!Contains(availableExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't find debug utils extension");
        }
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

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

        LoadVulkanInstanceExtensionFunctions(mInstance);

#if _DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
        debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsMessengerCreateInfo.pNext = nullptr;
        debugUtilsMessengerCreateInfo.flags = 0;
        debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsMessengerCreateInfo.pfnUserCallback = VulkanDebugCallback;
        debugUtilsMessengerCreateInfo.pUserData = nullptr;

        FASTCG_CHECK_VK_RESULT(VkExt::vkCreateDebugUtilsMessengerEXT(mInstance, &debugUtilsMessengerCreateInfo, mAllocationCallbacks.get(), &mDebugMessenger));
#endif
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

    void VulkanGraphicsSystem::CreateAllocator()
    {
        VmaAllocatorCreateInfo allocatorCreateInfo{};
        allocatorCreateInfo.flags = 0;
        allocatorCreateInfo.physicalDevice = mPhysicalDevice;
        allocatorCreateInfo.device = mDevice;
        allocatorCreateInfo.preferredLargeHeapBlockSize = 0;
        allocatorCreateInfo.pAllocationCallbacks = mAllocationCallbacks.get();
        allocatorCreateInfo.pDeviceMemoryCallbacks = nullptr;
        allocatorCreateInfo.pHeapSizeLimit = nullptr;
        allocatorCreateInfo.pVulkanFunctions = nullptr;
        allocatorCreateInfo.instance = mInstance;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        FASTCG_CHECK_VK_RESULT(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));
    }

    void VulkanGraphicsSystem::GetPhysicalDeviceProperties()
    {
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);

        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);

        for (VkFormat format = VK_FORMAT_UNDEFINED; format < LAST_FORMAT; format = (VkFormat)(((size_t)format) + 1))
        {
            vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &mFormatProperties[format]);
        }

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

        mPreTransform = surfaceCapabilities.currentTransform;

        mMaxSimultaneousFrames = std::max(std::min(mArgs.maxSimultaneousFrames, surfaceCapabilities.maxImageCount), surfaceCapabilities.minImageCount);
        assert(mMaxSimultaneousFrames > 0);

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
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: No present mode found");
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
        std::vector<VkImage> swapChainImages;
        swapChainImages.resize(swapChainCount);
        FASTCG_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(mDevice, mSwapChain, &swapChainCount, &swapChainImages[0]));

        TextureFormat format;
        BitsPerChannel bitsPerChannel;
        TextureDataType dataType;
        DecomposeVkFormat(mSwapChainSurfaceFormat.format, format, bitsPerChannel, dataType);
        VulkanTexture::Args args{"",
                                 mArgs.rScreenWidth,
                                 mArgs.rScreenHeight,
                                 TextureType::TEXTURE_2D,
                                 TextureUsageFlagBit::PRESENT,
                                 format,
                                 bitsPerChannel,
                                 dataType,
                                 TextureFilter::LINEAR_FILTER,
                                 TextureWrapMode::CLAMP,
                                 false};
        for (size_t i = 0; i < swapChainImages.size(); ++i)
        {
            const auto &rSwapChainImage = swapChainImages[i];
            args.name = "SwapChain Image " + std::to_string(i);
            args.image = rSwapChainImage;
            mSwapChainTextures.emplace_back(CreateTexture(args));
        }

        AcquireNextSwapChainImage();
    }

    void VulkanGraphicsSystem::AcquireNextSwapChainImage()
    {
        auto result = vkAcquireNextImageKHR(mDevice, mSwapChain, UINT64_MAX, mAcquireSwapChainImageSemaphores[mCurrentFrame], VK_NULL_HANDLE, &mSwapChainIndex);
        switch (result)
        {
        case VK_SUCCESS:
            break;
        case VK_SUBOPTIMAL_KHR:
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Outdated swapchain");
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't acquire new swapchain image");
            break;
        }
    }

    void VulkanGraphicsSystem::CreateSynchronizationObjects()
    {
        mFrameFences.resize(mMaxSimultaneousFrames);
        mAcquireSwapChainImageSemaphores.resize(mMaxSimultaneousFrames);
        mSubmitFinishedSemaphores.resize(mMaxSimultaneousFrames);

        VkFenceCreateInfo fenceInfo;
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = 0;

        VkSemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;
        for (uint32_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            // Create all fences that are not the current frame fence in signaled state
            fenceInfo.flags = i != mCurrentFrame ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
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

    void VulkanGraphicsSystem::CreateDescriptorPool()
    {
        // TODO: make this less brittle and possibly dynamic
        const VkDescriptorPoolSize DESCRIPTOR_POOL_SIZES[] = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024}};

        VkDescriptorPoolCreateInfo descriptorPoolCreateInto;
        descriptorPoolCreateInto.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInto.pNext = nullptr;
        descriptorPoolCreateInto.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInto.maxSets = 2048;
        descriptorPoolCreateInto.poolSizeCount = (uint32_t)FASTCG_ARRAYSIZE(DESCRIPTOR_POOL_SIZES);
        descriptorPoolCreateInto.pPoolSizes = DESCRIPTOR_POOL_SIZES;
        FASTCG_CHECK_VK_RESULT(vkCreateDescriptorPool(mDevice,
                                                      &descriptorPoolCreateInto,
                                                      mAllocationCallbacks.get(),
                                                      &mDescriptorPool));

        mDescriptorSetLocalPools.resize(mMaxSimultaneousFrames);
    }

    void VulkanGraphicsSystem::BeginCurrentCommandBuffer()
    {
        FASTCG_CHECK_VK_RESULT(vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0));

        VkCommandBufferBeginInfo commandBufferBeginInfo;
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;
        FASTCG_CHECK_VK_RESULT(vkBeginCommandBuffer(mCommandBuffers[mCurrentFrame], &commandBufferBeginInfo));
    }

    void VulkanGraphicsSystem::CreateImmediateGraphicsContext()
    {
        mpImmediateGraphicsContext = CreateGraphicsContext({"Immediate Graphics Context"});
        mpImmediateGraphicsContext->Begin();
    }

    void VulkanGraphicsSystem::EndCurrentCommandBuffer()
    {
        FASTCG_CHECK_VK_RESULT(vkEndCommandBuffer(mCommandBuffers[mCurrentFrame]));
    }

    void VulkanGraphicsSystem::DestroyFrameBuffers()
    {
        std::for_each(mFrameBuffers.begin(), mFrameBuffers.end(), [&](const auto &rEntry)
                      { vkDestroyFramebuffer(mDevice, rEntry.second, mAllocationCallbacks.get()); });
        mFrameBuffers.clear();
        mRenderTargetToFrameBufferHash.clear();
    }

    void VulkanGraphicsSystem::DestroyRenderPasses()
    {
        std::for_each(mRenderPasses.begin(), mRenderPasses.end(), [&](const auto &rEntry)
                      { vkDestroyRenderPass(mDevice, rEntry.second, mAllocationCallbacks.get()); });
        mRenderPasses.clear();
    }

    void VulkanGraphicsSystem::DestroyPipelineLayouts()
    {
        std::for_each(mPipelineLayouts.begin(), mPipelineLayouts.end(), [&](const auto &rEntry)
                      { vkDestroyPipelineLayout(mDevice, rEntry.second, mAllocationCallbacks.get()); });
        mPipelineLayouts.clear();
    }

    void VulkanGraphicsSystem::DestroyPipelines()
    {
        std::for_each(mPipelines.begin(), mPipelines.end(), [&](const auto &rEntry)
                      { vkDestroyPipeline(mDevice, rEntry.second, mAllocationCallbacks.get()); });
        mPipelines.clear();
    }

    void VulkanGraphicsSystem::DestroyDescriptorSetLayouts()
    {
        std::for_each(mDescriptorSetLayouts.begin(), mDescriptorSetLayouts.end(), [&](const auto &rEntry)
                      { vkDestroyDescriptorSetLayout(mDevice, rEntry.second, mAllocationCallbacks.get()); });
        mDescriptorSetLayouts.clear();
    }

    void VulkanGraphicsSystem::DestroyDescriptorPool()
    {
        if (mDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(mDevice, mDescriptorPool, mAllocationCallbacks.get());
            mDescriptorPool = VK_NULL_HANDLE;
        }
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
        for (const auto &pSwapChainTexture : mSwapChainTextures)
        {
            DestroyTexture(pSwapChainTexture);
        }
        mSwapChainTextures.clear();
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

    void VulkanGraphicsSystem::DestroyAllocator()
    {
        if (mAllocator != VK_NULL_HANDLE)
        {
            vmaDestroyAllocator(mAllocator);
            mAllocator = VK_NULL_HANDLE;
        }
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
#if _DEBUG
            VkExt::vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
#endif

            vkDestroyInstance(mInstance, mAllocationCallbacks.get());
            mInstance = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsSystem::Present()
    {
        auto imageMemoryTransition = GetLastImageMemoryTransition(GetCurrentSwapChainTexture());
        if (imageMemoryTransition.layout != VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            GetImmediateGraphicsContext()->AddTextureMemoryBarrier(GetCurrentSwapChainTexture(),
                                                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                                   imageMemoryTransition.accessMask,
                                                                   0,
                                                                   imageMemoryTransition.stageMask,
                                                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        }

        GetImmediateGraphicsContext()->End();

        EndCurrentCommandBuffer();

        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &mAcquireSwapChainImageSemaphores[mCurrentFrame];
        submitInfo.pWaitDstStageMask = &waitDstStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &mSubmitFinishedSemaphores[mCurrentFrame];
        if (vkQueueSubmit(mGraphicsAndPresentQueue, 1, &submitInfo, mFrameFences[mCurrentFrame]) != VK_SUCCESS)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't submit commands");
        }

        VkPresentInfoKHR presentInfo;
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &mSubmitFinishedSemaphores[mCurrentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &mSwapChain;
        presentInfo.pImageIndices = &mSwapChainIndex;
        presentInfo.pResults = nullptr;

        bool outdatedSwapchain = false;
        auto result = vkQueuePresentKHR(mGraphicsAndPresentQueue, &presentInfo);
        switch (result)
        {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            outdatedSwapchain = true;
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't present");
            break;
        }

        mCurrentFrame = (mCurrentFrame + 1) % mMaxSimultaneousFrames;

        FASTCG_CHECK_VK_RESULT(vkWaitForFences(mDevice, 1, &mFrameFences[mCurrentFrame], VK_TRUE, UINT64_MAX));
        FASTCG_CHECK_VK_RESULT(vkResetFences(mDevice, 1, &mFrameFences[mCurrentFrame]));

        PerformDeferredDestroys();

        for (auto &rEntry : mDescriptorSetLocalPools[mCurrentFrame])
        {
            rEntry.second.lastDescriptorSetIdx = 0;
        }

        BeginCurrentCommandBuffer();

        GetImmediateGraphicsContext()->Begin();

        if (outdatedSwapchain)
        {
            RecreateSwapChainAndGetImages();
        }
        else
        {
            AcquireNextSwapChainImage();
        }
    }

    double VulkanGraphicsSystem::GetPresentElapsedTime() const
    {
        return 0;
    }

    double VulkanGraphicsSystem::GetGpuElapsedTime() const
    {
        return 0;
    }

    std::pair<size_t, VkRenderPass> VulkanGraphicsSystem::GetOrCreateRenderPass(const VulkanRenderPassDescription &rRenderPassDescription,
                                                                                const std::vector<VulkanClearRequest> &rClearRequests,
                                                                                bool depthStencilWrite)
    {
        std::vector<AttachmentDefinition> attachmentDefinitions;
        std::for_each(rRenderPassDescription.renderTargets.begin(), rRenderPassDescription.renderTargets.end(), [&](const auto *pRenderTarget)
                      {
                        if (pRenderTarget == nullptr)
                        {
                            return;
                        }

                        attachmentDefinitions.emplace_back();
                        auto& rAttachmentDefinition = attachmentDefinitions.back();
                        rAttachmentDefinition.pTexture = pRenderTarget;
                        auto it = std::find_if(rClearRequests.begin(), rClearRequests.end(), [pRenderTarget](const auto &rClearRequest)
                                                { return rClearRequest.pTexture == pRenderTarget; });

                        if (it != rClearRequests.end() && (it->flags & VulkanClearRequestFlagBit::COLOR_OR_DEPTH) != 0)
                        {
                            rAttachmentDefinition.colorOrDepth = VK_ATTACHMENT_LOAD_OP_CLEAR;
                        }
                        else
                        {
                            rAttachmentDefinition.colorOrDepth = VK_ATTACHMENT_LOAD_OP_LOAD;
                        } 
                        rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_DONT_CARE; 
                        rAttachmentDefinition.write = true; });

        if (rRenderPassDescription.pDepthStencilBuffer != nullptr)
        {
            attachmentDefinitions.emplace_back();
            auto &rAttachmentDefinition = attachmentDefinitions.back();
            rAttachmentDefinition.pTexture = rRenderPassDescription.pDepthStencilBuffer;
            auto it = std::find_if(rClearRequests.begin(), rClearRequests.end(), [&rRenderPassDescription](const auto &rClearRequest)
                                   { return rClearRequest.pTexture == rRenderPassDescription.pDepthStencilBuffer; });

            if (it != rClearRequests.end() && (it->flags & VulkanClearRequestFlagBit::COLOR_OR_DEPTH) != 0)
            {
                rAttachmentDefinition.colorOrDepth = VK_ATTACHMENT_LOAD_OP_CLEAR;
            }
            else
            {
                rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_LOAD;
            }

            if (HasStencil(rRenderPassDescription.pDepthStencilBuffer->GetFormat()))
            {
                if (it != rClearRequests.end() && (it->flags & VulkanClearRequestFlagBit::STENCIL) != 0)
                {
                    rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_CLEAR;
                }
                else
                {
                    rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_LOAD;
                }
            }
            else
            {
                rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            }
            rAttachmentDefinition.write = depthStencilWrite;
        }

        auto renderPassHash = GetRenderPassHash(attachmentDefinitions);
        auto it = mRenderPasses.find(renderPassHash);
        if (it != mRenderPasses.end())
        {
            return {it->first, it->second};
        }

        VkRenderPassCreateInfo2 renderPassCreateInfo;
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
        renderPassCreateInfo.pNext = nullptr;
        renderPassCreateInfo.flags = 0;
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = nullptr;
        renderPassCreateInfo.correlatedViewMaskCount = 0;
        renderPassCreateInfo.pCorrelatedViewMasks = nullptr;

        VkSubpassDescription2 subpassDescription;
        subpassDescription.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
        subpassDescription.pNext = nullptr;
        subpassDescription.flags = 0;
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.viewMask = 0;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pInputAttachments = nullptr;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments = nullptr;
        subpassDescription.pResolveAttachments = nullptr;

        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;

        std::vector<VkAttachmentDescription2> attachmentDescriptions;
        std::vector<VkAttachmentReference2> colorAttachmentReferences;

        subpassDescription.pDepthStencilAttachment = nullptr;

        for (const auto *pRenderTarget : rRenderPassDescription.renderTargets)
        {
            if (pRenderTarget == nullptr)
            {
                continue;
            }

            auto attachmentIdx = (uint32_t)attachmentDescriptions.size();

            auto &rAttachmentDefinition = attachmentDefinitions[attachmentIdx];

            attachmentDescriptions.emplace_back();
            auto &rAttachmentDescription = attachmentDescriptions.back();
            rAttachmentDescription.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
            rAttachmentDescription.pNext = nullptr;
            rAttachmentDescription.flags = 0;
            rAttachmentDescription.format = pRenderTarget->GetVulkanFormat();
            rAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            rAttachmentDescription.loadOp = rAttachmentDefinition.colorOrDepth;
            rAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            rAttachmentDescription.stencilLoadOp = rAttachmentDefinition.stencil;
            rAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            rAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            rAttachmentDescription.finalLayout = pRenderTarget->GetRestingLayout();

            colorAttachmentReferences.emplace_back();
            auto &rColorAttachmentReference = colorAttachmentReferences.back();
            rColorAttachmentReference.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
            rColorAttachmentReference.pNext = nullptr;
            rColorAttachmentReference.attachment = attachmentIdx;
            rColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            rColorAttachmentReference.aspectMask = pRenderTarget->GetAspectFlags();
        }

        VkAttachmentReference2 depthAttachmentReference;
        if (rRenderPassDescription.pDepthStencilBuffer != nullptr)
        {
            auto attachmentIdx = (uint32_t)attachmentDescriptions.size();

            auto &rAttachmentDefinition = attachmentDefinitions[attachmentIdx];

            attachmentDescriptions.emplace_back();
            auto &rAttachmentDescription = attachmentDescriptions.back();
            rAttachmentDescription.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
            rAttachmentDescription.pNext = nullptr;
            rAttachmentDescription.flags = 0;
            rAttachmentDescription.format = rRenderPassDescription.pDepthStencilBuffer->GetVulkanFormat();
            rAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            rAttachmentDescription.loadOp = rAttachmentDefinition.colorOrDepth;
            rAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkImageLayout initialLayout, finalLayout;
            if (depthStencilWrite)
            {
                initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                finalLayout = rRenderPassDescription.pDepthStencilBuffer->GetRestingLayout();
            }
            else
            {
                initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            }

            depthAttachmentReference.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
            depthAttachmentReference.pNext = nullptr;
            depthAttachmentReference.attachment = attachmentIdx;
            depthAttachmentReference.layout = initialLayout;
            depthAttachmentReference.aspectMask = rRenderPassDescription.pDepthStencilBuffer->GetAspectFlags();

            rAttachmentDescription.stencilLoadOp = rAttachmentDefinition.stencil;
            if (HasStencil(rRenderPassDescription.pDepthStencilBuffer->GetFormat()))
            {
                rAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            }
            else
            {
                rAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            }
            rAttachmentDescription.initialLayout = initialLayout;
            rAttachmentDescription.finalLayout = finalLayout;

            subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
        }

        renderPassCreateInfo.attachmentCount = (uint32_t)attachmentDescriptions.size();
        renderPassCreateInfo.pAttachments = &attachmentDescriptions[0];

        subpassDescription.colorAttachmentCount = (uint32_t)colorAttachmentReferences.size();
        subpassDescription.pColorAttachments = colorAttachmentReferences.empty() ? nullptr : &colorAttachmentReferences[0];

        VkRenderPass renderPass;
        FASTCG_CHECK_VK_RESULT(vkCreateRenderPass2(mDevice,
                                                   &renderPassCreateInfo,
                                                   mAllocationCallbacks.get(),
                                                   &renderPass));

        it = mRenderPasses.emplace(renderPassHash, renderPass).first;

        return {it->first, it->second};
    }

    std::pair<size_t, VkFramebuffer> VulkanGraphicsSystem::GetOrCreateFrameBuffer(const VulkanRenderPassDescription &rRenderPassDescription,
                                                                                  const std::vector<VulkanClearRequest> &rClearRequests,
                                                                                  bool depthStencilWrite)
    {
        auto result = GetOrCreateRenderPass(rRenderPassDescription, rClearRequests, depthStencilWrite);

        auto it = mFrameBuffers.find(result.first);
        if (it != mFrameBuffers.end())
        {
            return {it->first, it->second};
        }

        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = nullptr;
        framebufferCreateInfo.flags = 0;
        framebufferCreateInfo.renderPass = result.second;
        framebufferCreateInfo.layers = 1;

        std::vector<VkImageView> attachments;

        framebufferCreateInfo.width = std::numeric_limits<uint32_t>::max();
        framebufferCreateInfo.height = std::numeric_limits<uint32_t>::max();
        auto CalculateFramebufferDimensions = [&framebufferCreateInfo](const VulkanTexture *pRenderTarget)
        {
            // according to VUID-VkFramebufferCreateInfo-flags-04533/0433, each element of pAttachments (...)
            // must have been created with a (...) width/height greater than or equal the width/height of the
            // framebuffer
            framebufferCreateInfo.width = std::min(framebufferCreateInfo.width, pRenderTarget->GetWidth());
            framebufferCreateInfo.height = std::min(framebufferCreateInfo.height, pRenderTarget->GetHeight());
        };

        for (const auto *pRenderTarget : rRenderPassDescription.renderTargets)
        {
            if (pRenderTarget == nullptr)
            {
                continue;
            }

            CalculateFramebufferDimensions(pRenderTarget);

            attachments.emplace_back(pRenderTarget->GetDefaultImageView());
        }

        if (rRenderPassDescription.pDepthStencilBuffer != nullptr)
        {
            CalculateFramebufferDimensions(rRenderPassDescription.pDepthStencilBuffer);

            attachments.emplace_back(rRenderPassDescription.pDepthStencilBuffer->GetDefaultImageView());
        }

        framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
        framebufferCreateInfo.pAttachments = &attachments[0];

        VkFramebuffer frameBuffer;
        FASTCG_CHECK_VK_RESULT(vkCreateFramebuffer(mDevice,
                                                   &framebufferCreateInfo,
                                                   mAllocationCallbacks.get(),
                                                   &frameBuffer));

        it = mFrameBuffers.emplace(result.first, frameBuffer).first;

        for (const auto *pRenderTarget : rRenderPassDescription.renderTargets)
        {
            if (pRenderTarget != nullptr)
            {
                auto image = pRenderTarget->GetImage();
                mRenderTargetToFrameBufferHash[image] = result.first;
                mFrameBufferHashToRenderTargets[result.first].emplace_back(image);
            }
        }

        return {it->first, it->second};
    }

    std::pair<size_t, VulkanPipeline> VulkanGraphicsSystem::GetOrCreatePipeline(const VulkanPipelineDescription &rPipelineDescription,
                                                                                VkRenderPass renderPass,
                                                                                uint32_t renderTargetCount,
                                                                                const std::vector<const VulkanBuffer *> &rVertexBuffers)
    {
        assert(rPipelineDescription.pShader != nullptr);

        auto pipelineLayout = GetOrCreatePipelineLayout(rPipelineDescription.pShader->GetLayoutDescription()).second;

        auto pipelineHash = GetPipelineHash(rPipelineDescription);

        auto it = mPipelines.find(pipelineHash);
        if (it != mPipelines.end())
        {
            return {it->first, {it->second, pipelineLayout}};
        }

        VkGraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = nullptr;
        pipelineCreateInfo.flags = 0;

        pipelineCreateInfo.stageCount = 0;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfos[(ShaderTypeInt)ShaderType::LAST] = {};
        pipelineCreateInfo.pStages = shaderStageCreateInfos;

        for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::LAST; ++i)
        {
            auto shaderType = (ShaderType)i;
            auto module = rPipelineDescription.pShader->GetModule(shaderType);
            if (module != VK_NULL_HANDLE)
            {
                auto &rShaderStageCreateInfo = shaderStageCreateInfos[pipelineCreateInfo.stageCount++];

                rShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                rShaderStageCreateInfo.pNext = nullptr;
                rShaderStageCreateInfo.flags = 0;
                rShaderStageCreateInfo.pName = "main";
                rShaderStageCreateInfo.module = module;
                rShaderStageCreateInfo.stage = GetVkShaderStageFlagBit(shaderType);
                rShaderStageCreateInfo.pSpecializationInfo = NULL;
            }
        }

        const auto &rVertexInputDescription = rPipelineDescription.pShader->GetVertexInputDescription();
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
        std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
        for (size_t i = 0; i < rVertexBuffers.size(); ++i)
        {
            const auto *pVertexBuffer = rVertexBuffers[i];
            for (const auto &rVbDesc : pVertexBuffer->GetVertexBindingDescriptors())
            {
                auto it = rVertexInputDescription.find(rVbDesc.binding);
                if (it == rVertexInputDescription.end())
                {
                    continue;
                }
                auto format = GetVkFormat(rVbDesc.type, rVbDesc.size);
                vertexInputAttributeDescriptions.emplace_back(VkVertexInputAttributeDescription{rVbDesc.binding, (uint32_t)i, format, rVbDesc.offset});
                auto stride = rVbDesc.stride == 0 ? GetVkStride(format) : rVbDesc.stride;
                vertexInputBindingDescriptions.emplace_back(VkVertexInputBindingDescription{rVbDesc.binding, stride, VK_VERTEX_INPUT_RATE_VERTEX});
            }
        }

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.pNext = nullptr;
        vertexInputStateCreateInfo.flags = 0;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescriptions[0];
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescriptions.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = &vertexInputAttributeDescriptions[0];
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexInputAttributeDescriptions.size();
        pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
        inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCreateInfo.pNext = nullptr;
        inputAssemblyCreateInfo.flags = 0;
        inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // TODO: support other topologies
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

        pipelineCreateInfo.pTessellationState = nullptr; // TODO: support tessellation shaders

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.pNext = nullptr;
        viewportStateCreateInfo.flags = 0;
        viewportStateCreateInfo.pViewports = nullptr;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pScissors = nullptr;
        viewportStateCreateInfo.scissorCount = 1;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.pNext = nullptr;
        rasterizationStateCreateInfo.flags = 0;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.cullMode = GetVkCullModeFlags(rPipelineDescription.cullMode);
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0;
        rasterizationStateCreateInfo.depthBiasClamp = 0;
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
        rasterizationStateCreateInfo.lineWidth = 1;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;

        VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo;
        multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSampleStateCreateInfo.pNext = nullptr;
        multiSampleStateCreateInfo.flags = 0;
        multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multiSampleStateCreateInfo.minSampleShading = 0;
        multiSampleStateCreateInfo.pSampleMask = nullptr;
        multiSampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multiSampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
        pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.pNext = nullptr;
        depthStencilStateCreateInfo.flags = 0;
        depthStencilStateCreateInfo.depthTestEnable = rPipelineDescription.depthTest;
        depthStencilStateCreateInfo.depthWriteEnable = rPipelineDescription.depthWrite;
        depthStencilStateCreateInfo.depthCompareOp = GetVkCompareOp(rPipelineDescription.depthFunc);
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.stencilTestEnable = rPipelineDescription.stencilTest;
        depthStencilStateCreateInfo.front.compareMask = rPipelineDescription.stencilFrontState.compareMask;
        depthStencilStateCreateInfo.front.compareOp = GetVkCompareOp(rPipelineDescription.stencilFrontState.compareOp);
        depthStencilStateCreateInfo.front.depthFailOp = GetVkStencilOp(rPipelineDescription.stencilFrontState.depthFailOp);
        depthStencilStateCreateInfo.front.failOp = GetVkStencilOp(rPipelineDescription.stencilFrontState.stencilFailOp);
        depthStencilStateCreateInfo.front.passOp = GetVkStencilOp(rPipelineDescription.stencilFrontState.passOp);
        depthStencilStateCreateInfo.front.reference = rPipelineDescription.stencilFrontState.reference;
        depthStencilStateCreateInfo.front.writeMask = rPipelineDescription.stencilFrontState.writeMask;
        depthStencilStateCreateInfo.back.compareMask = rPipelineDescription.stencilBackState.compareMask;
        depthStencilStateCreateInfo.back.compareOp = GetVkCompareOp(rPipelineDescription.stencilBackState.compareOp);
        depthStencilStateCreateInfo.back.depthFailOp = GetVkStencilOp(rPipelineDescription.stencilBackState.depthFailOp);
        depthStencilStateCreateInfo.back.failOp = GetVkStencilOp(rPipelineDescription.stencilBackState.stencilFailOp);
        depthStencilStateCreateInfo.back.passOp = GetVkStencilOp(rPipelineDescription.stencilBackState.passOp);
        depthStencilStateCreateInfo.back.reference = rPipelineDescription.stencilBackState.reference;
        depthStencilStateCreateInfo.back.writeMask = rPipelineDescription.stencilBackState.writeMask;
        depthStencilStateCreateInfo.minDepthBounds = 0;
        depthStencilStateCreateInfo.maxDepthBounds = 1;
        pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        colorBlendAttachmentStates.resize(renderTargetCount);
        for (auto &rColorBlendAttachmentState : colorBlendAttachmentStates)
        {
            rColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            rColorBlendAttachmentState.blendEnable = rPipelineDescription.blend;
            rColorBlendAttachmentState.colorBlendOp = GetVkBlendOp(rPipelineDescription.blendState.colorOp);
            rColorBlendAttachmentState.srcColorBlendFactor = GetVkBlendFactor(rPipelineDescription.blendState.srcColorFactor);
            rColorBlendAttachmentState.dstColorBlendFactor = GetVkBlendFactor(rPipelineDescription.blendState.dstColorFactor);
            rColorBlendAttachmentState.alphaBlendOp = GetVkBlendOp(rPipelineDescription.blendState.alphaOp);
            rColorBlendAttachmentState.srcAlphaBlendFactor = GetVkBlendFactor(rPipelineDescription.blendState.srcAlphaFactor);
            rColorBlendAttachmentState.dstAlphaBlendFactor = GetVkBlendFactor(rPipelineDescription.blendState.dstAlphaFactor);
        }

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.pNext = nullptr;
        colorBlendStateCreateInfo.flags = 0;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
        colorBlendStateCreateInfo.attachmentCount = (uint32_t)colorBlendAttachmentStates.size();
        colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentStates.empty() ? nullptr : &colorBlendAttachmentStates[0];
        colorBlendStateCreateInfo.blendConstants[0] = 0;
        colorBlendStateCreateInfo.blendConstants[1] = 0;
        colorBlendStateCreateInfo.blendConstants[2] = 0;
        colorBlendStateCreateInfo.blendConstants[3] = 0;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

        const VkDynamicState DYNAMIC_STATES[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.pNext = nullptr;
        dynamicStateCreateInfo.flags = 0;
        dynamicStateCreateInfo.pDynamicStates = DYNAMIC_STATES;
        dynamicStateCreateInfo.dynamicStateCount = FASTCG_ARRAYSIZE(DYNAMIC_STATES);
        pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = 0;

        VkPipeline pipeline;
        FASTCG_CHECK_VK_RESULT(vkCreateGraphicsPipelines(mDevice,
                                                         nullptr,
                                                         1,
                                                         &pipelineCreateInfo,
                                                         mAllocationCallbacks.get(),
                                                         &pipeline));

        it = mPipelines.emplace(pipelineHash, pipeline).first;

        return {it->first, {it->second, pipelineLayout}};
    }

    std::pair<size_t, VkPipelineLayout> VulkanGraphicsSystem::GetOrCreatePipelineLayout(const VulkanPipelineLayoutDescription &rPipelineLayoutDescription)
    {
        auto pipelineLayoutHash = GetPipelineLayoutHash(rPipelineLayoutDescription);

        auto it = mPipelineLayouts.find(pipelineLayoutHash);
        if (it != mPipelineLayouts.end())
        {
            return {it->first, it->second};
        }

        std::vector<VkDescriptorSetLayout> setLayouts;
        for (const auto &rSetLayout : rPipelineLayoutDescription)
        {
            if (!rSetLayout.empty())
            {
                setLayouts.emplace_back(GetOrCreateDescriptorSetLayout(rSetLayout).second);
            }
            else
            {
                setLayouts.emplace_back(VK_NULL_HANDLE);
            }
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = nullptr;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)setLayouts.size();
        pipelineLayoutCreateInfo.pSetLayouts = &setLayouts[0];
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        VkPipelineLayout pipelineLayout;
        FASTCG_CHECK_VK_RESULT(vkCreatePipelineLayout(VulkanGraphicsSystem::GetInstance()->GetDevice(),
                                                      &pipelineLayoutCreateInfo,
                                                      VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(),
                                                      &pipelineLayout));

        it = mPipelineLayouts.emplace(pipelineLayoutHash, pipelineLayout).first;

        return {it->first, it->second};
    }

    std::pair<size_t, VkDescriptorSetLayout> VulkanGraphicsSystem::GetOrCreateDescriptorSetLayout(const VulkanDescriptorSetLayout &rDescriptorSetLayout)
    {
        assert(!rDescriptorSetLayout.empty());

        auto setLayoutHash = GetDescriptorSetLayoutHash(rDescriptorSetLayout);

        auto it = mDescriptorSetLayouts.find(setLayoutHash);
        if (it != mDescriptorSetLayouts.end())
        {
            return {it->first, it->second};
        }

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        for (const auto &rBinding : rDescriptorSetLayout)
        {
            VkDescriptorSetLayoutBinding layoutBinding;
            layoutBinding.binding = rBinding.binding;
            layoutBinding.descriptorCount = 1;
            layoutBinding.descriptorType = rBinding.type;
            layoutBinding.pImmutableSamplers = nullptr;
            layoutBinding.stageFlags = rBinding.stageFlags;
            layoutBindings.emplace_back(layoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo;
        setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutCreateInfo.pNext = nullptr;
        setLayoutCreateInfo.flags = 0;
        setLayoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
        setLayoutCreateInfo.pBindings = &layoutBindings[0];

        VkDescriptorSetLayout setLayout;
        FASTCG_CHECK_VK_RESULT(vkCreateDescriptorSetLayout(VulkanGraphicsSystem::GetInstance()->GetDevice(),
                                                           &setLayoutCreateInfo,
                                                           VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(),
                                                           &setLayout));

        it = mDescriptorSetLayouts.emplace(setLayoutHash, setLayout).first;

        return {it->first, it->second};
    }

    std::pair<size_t, VkDescriptorSet> VulkanGraphicsSystem::GetOrCreateDescriptorSet(const VulkanDescriptorSetLayout &rDescriptorSetLayout)
    {
        auto setLayoutHash = GetDescriptorSetLayoutHash(rDescriptorSetLayout);

        auto &rDescriptorSetLocalPools = mDescriptorSetLocalPools[mCurrentFrame];
        auto it = rDescriptorSetLocalPools.find(setLayoutHash);
        if (it != rDescriptorSetLocalPools.end())
        {
            auto &rDescriptorSetLocalPool = it->second;
            auto descriptorSet = rDescriptorSetLocalPool.descriptorSets[rDescriptorSetLocalPool.lastDescriptorSetIdx++];
            assert(rDescriptorSetLocalPool.lastDescriptorSetIdx < DescriptorSetLocalPool::MAX_SETS);
            return {it->first, descriptorSet};
        }

        auto setLayout = GetOrCreateDescriptorSetLayout(rDescriptorSetLayout).second;

        auto &rDescriptorSetLocalPool = rDescriptorSetLocalPools[setLayoutHash];

        for (size_t i = 0; i < DescriptorSetLocalPool::MAX_SETS; ++i)
        {
            auto &rDescriptorSet = rDescriptorSetLocalPool.descriptorSets[i];

            VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
            descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.pNext = nullptr;
            descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
            descriptorSetAllocateInfo.descriptorSetCount = 1;
            descriptorSetAllocateInfo.pSetLayouts = &setLayout;

            FASTCG_CHECK_VK_RESULT(vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &rDescriptorSet));
        }

        auto descriptorSet = rDescriptorSetLocalPool.descriptorSets[rDescriptorSetLocalPool.lastDescriptorSetIdx++];
        assert(rDescriptorSetLocalPool.lastDescriptorSetIdx < DescriptorSetLocalPool::MAX_SETS);
        return {setLayoutHash, descriptorSet};
    }

#ifdef _DEBUG
    void VulkanGraphicsSystem::PushDebugMarker(VkCommandBuffer commandBuffer, const char *pName)
    {
        VkDebugUtilsLabelEXT debugLabel;
        debugLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        debugLabel.pNext = nullptr;
        debugLabel.pLabelName = pName;
        debugLabel.color[0] = 0;
        debugLabel.color[1] = 0;
        debugLabel.color[2] = 1.0f;
        debugLabel.color[3] = 1.0f;
        VkExt::vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &debugLabel);
    }

    void VulkanGraphicsSystem::PopDebugMarker(VkCommandBuffer commandBuffer)
    {
        VkExt::vkCmdEndDebugUtilsLabelEXT(commandBuffer);
    }

    void VulkanGraphicsSystem::SetObjectName(const char *pObjectName, VkObjectType objectType, uint64_t objectHandle)
    {
        VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo;
        debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugUtilsObjectNameInfo.pNext = nullptr;
        debugUtilsObjectNameInfo.objectType = objectType;
        debugUtilsObjectNameInfo.objectHandle = objectHandle;
        debugUtilsObjectNameInfo.pObjectName = pObjectName;
        FASTCG_CHECK_VK_RESULT(VkExt::vkSetDebugUtilsObjectNameEXT(mDevice, &debugUtilsObjectNameInfo));
    }
#endif

    void VulkanGraphicsSystem::PerformDeferredDestroys()
    {
        while (!mDeferredDestroyRequests.empty())
        {
            const auto &rDeferredDestroyRequest = mDeferredDestroyRequests.front();
            if (rDeferredDestroyRequest.frame != mCurrentFrame)
            {
                break;
            }

            switch (rDeferredDestroyRequest.type)
            {
            case DeferredDestroyRequest::Type::BUFFER:
                Super::DestroyBuffer(rDeferredDestroyRequest.pBuffer);
                break;
            case DeferredDestroyRequest::Type::SHADER:
                Super::DestroyShader(rDeferredDestroyRequest.pShader);
                break;
            case DeferredDestroyRequest::Type::TEXTURE:
                Super::DestroyTexture(rDeferredDestroyRequest.pTexture);
                break;
            case DeferredDestroyRequest::Type::FRAME_BUFFER:
                vkDestroyFramebuffer(mDevice, rDeferredDestroyRequest.frameBuffer, mAllocationCallbacks.get());
                break;
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled deferred destroy type %d", (int)rDeferredDestroyRequest.type);
                break;
            }

            mDeferredDestroyRequests.pop_front();
        }
    }

    void VulkanGraphicsSystem::FinalizeDeferredDestroys()
    {
        for (auto &rDeferredDestroyRequest : mDeferredDestroyRequests)
        {
            switch (rDeferredDestroyRequest.type)
            {
            case DeferredDestroyRequest::Type::BUFFER:
            case DeferredDestroyRequest::Type::SHADER:
            case DeferredDestroyRequest::Type::TEXTURE:
                // ignore those deferred destroy requests
                break;
            case DeferredDestroyRequest::Type::FRAME_BUFFER:
                vkDestroyFramebuffer(mDevice, rDeferredDestroyRequest.frameBuffer, mAllocationCallbacks.get());
                break;
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled deferred destroy type %d", (int)rDeferredDestroyRequest.type);
                break;
            }
        }
        mDeferredDestroyRequests.clear();
    }
}

#endif