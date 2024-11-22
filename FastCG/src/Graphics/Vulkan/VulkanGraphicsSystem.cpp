#ifdef FASTCG_VULKAN

#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Version.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>
#include <FastCG/Platform/Application.h>

#include <algorithm>
#include <cassert>
#include <limits>
#include <sstream>
#include <string>

namespace
{
    template <typename AType>
    struct StrComparer;

    template <>
    struct StrComparer<const char *>
    {
        static bool Compare(const char *a, const char *b)
        {
            return strcmp(a, b) == 0;
        }
    };

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
        return std::find_if(vector.begin(), vector.end(), [&value](const auto &element) {
                   return StrComparer<ElementType>::Compare(element, value);
               }) != vector.end();
    }

    bool SupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIdx)
    {
#if defined FASTCG_WINDOWS
        return vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIdx);
#elif defined FASTCG_LINUX
        auto *pDisplay = FastCG::X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);
        // uses visual ID from default visual. Only works because we're using a "simple window".
        auto visualId = XVisualIDFromVisual(DefaultVisual(pDisplay, DefaultScreen(pDisplay)));
        return vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIdx, pDisplay, visualId);
#elif defined FASTCG_ANDROID
        // TODO: apparently, there's no need for checking whether a queue family supports presentation on Android
        return true;
#else
#error "FastCG::SupportsPresentation(): Don't know how to check presentation support"
#endif
        return false;
    }

    bool SupportsPresentationToSurface(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIdx, VkSurfaceKHR surface)
    {
        VkBool32 supportsPresentationToSurface;
        FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIdx, surface,
                                                                    &supportsPresentationToSurface));
        return supportsPresentationToSurface;
    }

#if _DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                              void *pUserData)
    {
        std::stringstream stream;
        stream << "[VULKAN]"
               << " - " << FastCG::GetVkDebugUtilsMessageSeverityFlagBitsString(messageSeverity);

        // if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
        // {
        //     FASTCG_BREAK_TO_DEBUGGER();
        // }

        bool prevType = false;
        if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) != 0)
        {
            stream << " - "
                   << FastCG::GetVkDebugUtilsMessageTypeFlagBitsString(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
            prevType = true;
        }
        if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0)
        {
            stream << (prevType ? "|" : " - ")
                   << FastCG::GetVkDebugUtilsMessageTypeFlagBitsString(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
            prevType = true;
        }
        if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0)
        {
            stream << (prevType ? "|" : " - ")
                   << FastCG::GetVkDebugUtilsMessageTypeFlagBitsString(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
            prevType = true;
        }
        stream << " - " << pCallbackData->messageIdNumber << " - " << pCallbackData->pMessage << std::endl;

        FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "%s", stream.str().c_str());

        return VK_FALSE;
    }
#endif

#define FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, fn)                                                                   \
    FastCG::VkExt::fn = (PFN_##fn)vkGetInstanceProcAddr(instance, #fn);                                                \
    if (FastCG::VkExt::fn == nullptr)                                                                                  \
    {                                                                                                                  \
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: Couldn't load instance extension function " #fn);           \
    }

    void LoadVulkanInstanceExtensionFunctions(VkInstance instance, const std::vector<const char *> &rExtensions)
    {
#if _DEBUG
        if (Contains(rExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkCreateDebugUtilsMessengerEXT);
            FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkDestroyDebugUtilsMessengerEXT);
            FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkCmdBeginDebugUtilsLabelEXT);
            FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkCmdEndDebugUtilsLabelEXT);
            FASTCG_LOAD_VK_INSTANCE_EXT_FN(instance, vkSetDebugUtilsObjectNameEXT);
        }
#endif
    }

#undef FASTCG_LOAD_VK_INSTANCE_EXT_FN

#define FASTCG_LOAD_VK_DEVICE_EXT_FN(device, fn)                                                                       \
    FastCG::VkExt::fn = (PFN_##fn)vkGetDeviceProcAddr(device, #fn);                                                    \
    if (FastCG::VkExt::fn == nullptr)                                                                                  \
    {                                                                                                                  \
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: Couldn't load device extension function " #fn);             \
    }

    void LoadVulkanDeviceExtensionFunctions(VkDevice device)
    {
        FASTCG_LOAD_VK_DEVICE_EXT_FN(device, vkCreateRenderPass2KHR);
        FASTCG_LOAD_VK_DEVICE_EXT_FN(device, vkCmdBeginRenderPass2KHR);
        FASTCG_LOAD_VK_DEVICE_EXT_FN(device, vkCmdEndRenderPass2KHR);
    }

#undef FASTCG_LOAD_VK_DEVICE_EXT_FN

    // remove all padding cause struct memory is used to compute hash
    FASTCG_PACKED_PREFIX struct AttachmentDefinition
    {
        const FastCG::VulkanTexture *pTexture;
        VkAttachmentLoadOp colorOrDepth;
        VkAttachmentLoadOp stencil;
        uint8_t write : 2; // 0=no write, 1=write depth/colour, 2=write stencil, 3=write depth&stencil
    } FASTCG_PACKED_SUFFIX;

    size_t GetRenderPassHash(const std::vector<AttachmentDefinition> &rAttachmentDefinitions)
    {
        // TODO: use renderpass compatibility rule:
        // https://registry.khronos.org/vulkan/specs/1.1-extensions/html/vkspec.html#renderpass-compatibility
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rAttachmentDefinitions[0]),
                                     rAttachmentDefinitions.size() * sizeof(&rAttachmentDefinitions[0]));
    }

    size_t GetPipelineHash(const FastCG::VulkanPipelineDescription &rPipelineDescription)
    {
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rPipelineDescription),
                                     sizeof(rPipelineDescription));
    }

    size_t GetPipelineLayoutHash(const FastCG::VulkanPipelineLayoutDescription &rPipelineLayoutDescription)
    {
        assert(rPipelineLayoutDescription.setLayoutCount > 0);
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rPipelineLayoutDescription.pSetLayouts[0]),
                                     rPipelineLayoutDescription.setLayoutCount *
                                         sizeof(rPipelineLayoutDescription.pSetLayouts[0]));
    }

    size_t GetDescriptorSetLayoutHash(const FastCG::VulkanDescriptorSetLayout &rDescriptorSetLayout)
    {
        assert(rDescriptorSetLayout.bindingLayoutCount > 0);
        return (size_t)FastCG::FNV1a(reinterpret_cast<const uint8_t *>(&rDescriptorSetLayout.pBindingLayouts[0]),
                                     rDescriptorSetLayout.bindingLayoutCount *
                                         sizeof(rDescriptorSetLayout.pBindingLayouts[0]));
    }

}

// defined in the application's Config.cpp
extern const uint8_t APPLICATION_MAJOR_VERSION;
extern const uint8_t APPLICATION_MINOR_VERSION;
extern const uint8_t APPLICATION_PATCH_VERSION;
extern const char *const APPLICATION_NAME;

namespace FastCG
{
#define FASTCG_IMPL_VK_EXT_FN(fn) PFN_##fn fn = nullptr

    namespace VkExt
    {
        FASTCG_IMPL_VK_EXT_FN(vkCreateRenderPass2KHR);
        FASTCG_IMPL_VK_EXT_FN(vkCmdBeginRenderPass2KHR);
        FASTCG_IMPL_VK_EXT_FN(vkCmdEndRenderPass2KHR);
#if _DEBUG
        FASTCG_IMPL_VK_EXT_FN(vkCreateDebugUtilsMessengerEXT);
        FASTCG_IMPL_VK_EXT_FN(vkDestroyDebugUtilsMessengerEXT);
        FASTCG_IMPL_VK_EXT_FN(vkCmdBeginDebugUtilsLabelEXT);
        FASTCG_IMPL_VK_EXT_FN(vkCmdEndDebugUtilsLabelEXT);
        FASTCG_IMPL_VK_EXT_FN(vkSetDebugUtilsObjectNameEXT);
#endif
    }

#undef FASTCG_IMPL_VK_EXT_FN

    VulkanGraphicsSystem::VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs) : BaseGraphicsSystem(rArgs)
    {
    }

    VulkanGraphicsSystem::~VulkanGraphicsSystem() = default;

    void VulkanGraphicsSystem::OnInitialize()
    {
        BaseGraphicsSystem::OnInitialize();

        CreateInstance();
        SelectPhysicalDevice();
        AcquirePhysicalDeviceProperties();
        CreateDeviceAndGetQueues();
        CreateAllocator();
        CreateSynchronizationObjects();
        CreateCommandPoolAndCommandBuffers();
        CreateDescriptorPool();
        CreateQueryPool();
        BeginCurrentCommandBuffer();
        ResetQueryPool();
        CreateImmediateGraphicsContext();
        CreateSurfacelessSwapChain();
    }

    void VulkanGraphicsSystem::OnPreFinalize()
    {
        BaseGraphicsSystem::OnPreFinalize();

        // wait for device to become idle
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
        DestroySwapChain();
        DestroyQueryPool();
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
        applicationInfo.pApplicationName = APPLICATION_NAME;
        applicationInfo.applicationVersion =
            VK_MAKE_VERSION(APPLICATION_MAJOR_VERSION, APPLICATION_MINOR_VERSION, APPLICATION_PATCH_VERSION);
        applicationInfo.pEngineName = "FastCG";
        applicationInfo.engineVersion = VK_MAKE_VERSION(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
        applicationInfo.apiVersion = VK_API_VERSION;

#if _DEBUG
        uint32_t availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, &availableLayers[0]);

        FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "Available layers:");
        for (const auto &rLayer : availableLayers)
        {
            FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "- %s", rLayer.layerName);
        }
#endif

        std::vector<const char *> usedLayers;
#if _DEBUG && !defined FASTCG_DISABLE_GPU_VALIDATION
        if (Contains(availableLayers, "VK_LAYER_KHRONOS_validation"))
        {
            usedLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        }
        else
        {
            FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "VK_LAYER_KHRONOS_validation not available, ignoring it");
        }
#endif

        uint32_t instanceExtensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);

        mInstanceExtensionProperties.resize(instanceExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, &mInstanceExtensionProperties[0]);

#if _DEBUG
        FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "Available instance extensions:");
        for (const auto &rExtensionProperty : mInstanceExtensionProperties)
        {
            FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "- %s", rExtensionProperty.extensionName);
        }
#endif

        if (!Contains(mInstanceExtensionProperties, VK_KHR_SURFACE_EXTENSION_NAME))
        {
            FASTCG_THROW_EXCEPTION(Exception,
                                   "Vulkan: Couldn't find " VK_KHR_SURFACE_EXTENSION_NAME " instance extension");
        }
        mInstanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
        if (!mArgs.headless)
        {
            const char *pPlatformSurfaceExtName =
#if defined FASTCG_WINDOWS
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined FASTCG_LINUX
                VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#elif defined FASTCG_ANDROID
                "VK_KHR_android_surface"
#else
#error "FastCG::VulkanGraphicsSystem::CreateInstance(): Don't know how to enable surfaces in the current platform"
#endif
                ;
            if (!Contains(mInstanceExtensionProperties, pPlatformSurfaceExtName))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't find %s instance extension",
                                       pPlatformSurfaceExtName);
            }
            mInstanceExtensions.emplace_back(pPlatformSurfaceExtName);
        }

#if _DEBUG
        // VK_EXT_debug_utils is an extension of the validation layer
        if (Contains(availableLayers, "VK_LAYER_KHRONOS_validation"))
        {
            mInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
#endif

        VkInstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = (uint32_t)usedLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = usedLayers.empty() ? nullptr : &usedLayers[0];
        instanceCreateInfo.enabledExtensionCount = (uint32_t)mInstanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = mInstanceExtensions.empty() ? nullptr : &mInstanceExtensions[0];

        FASTCG_CHECK_VK_RESULT(vkCreateInstance(&instanceCreateInfo, mAllocationCallbacks.get(), &mInstance));

        LoadVulkanInstanceExtensionFunctions(mInstance, mInstanceExtensions);

#if _DEBUG
        if (Contains(mInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
            debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugUtilsMessengerCreateInfo.pNext = nullptr;
            debugUtilsMessengerCreateInfo.flags = 0;
            debugUtilsMessengerCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugUtilsMessengerCreateInfo.pfnUserCallback = VulkanDebugCallback;
            debugUtilsMessengerCreateInfo.pUserData = nullptr;

            FASTCG_CHECK_VK_RESULT(VkExt::vkCreateDebugUtilsMessengerEXT(mInstance, &debugUtilsMessengerCreateInfo,
                                                                         mAllocationCallbacks.get(), &mDebugMessenger));
        }
#endif
    }

    void VulkanGraphicsSystem::CreateSurface(void *pWindow)
    {
        DestroySurface();

#if defined FASTCG_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
        assert(surfaceCreateInfo.hinstance != nullptr);
        surfaceCreateInfo.hwnd = (HWND)pWindow;
        assert(surfaceCreateInfo.hwnd != nullptr);
        FASTCG_CHECK_VK_RESULT(
            vkCreateWin32SurfaceKHR(mInstance, &surfaceCreateInfo, mAllocationCallbacks.get(), &mSurface));
#elif defined FASTCG_LINUX
        VkXlibSurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.dpy = X11Application::GetInstance()->GetDisplay();
        assert(surfaceCreateInfo.dpy != nullptr);
        surfaceCreateInfo.window = (Window)pWindow;
        assert(surfaceCreateInfo.window != None);
        FASTCG_CHECK_VK_RESULT(
            vkCreateXlibSurfaceKHR(mInstance, &surfaceCreateInfo, mAllocationCallbacks.get(), &mSurface));
#elif defined FASTCG_ANDROID
        VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = NULL;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.window = (ANativeWindow *)pWindow;
        assert(surfaceCreateInfo.window != nullptr);
        FASTCG_CHECK_VK_RESULT(
            vkCreateAndroidSurfaceKHR(mInstance, &surfaceCreateInfo, mAllocationCallbacks.get(), &mSurface));
#else
#error "FastCG::VulkanGraphicsSystem::CreateSurface(): Don't know how to create a surface"
#endif

        if (!SupportsPresentationToSurface(mPhysicalDevice, mQueueFamilyIdx, mSurface))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Cannot present to surface");
        }
    }

    void VulkanGraphicsSystem::SelectPhysicalDevice()
    {
        uint32_t numPhysicalDevices;
        FASTCG_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(mInstance, &numPhysicalDevices, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices;
        physicalDevices.resize(numPhysicalDevices);
        FASTCG_CHECK_VK_RESULT(vkEnumeratePhysicalDevices(mInstance, &numPhysicalDevices, &physicalDevices[0]));

#if _DEBUG
        FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "Devices:");
        for (auto &rPhysicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(rPhysicalDevice, &properties);
            FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "- %s", properties.deviceName);
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
                if ((rQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                {
                    // FIXME: checking invariants
                    assert((rQueueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0);

                    mQueueSupportsPresentation = SupportsPresentation(rPhysicalDevice, queueFamilyIdx);
                    mQueueSupportsCompute = (rQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
                    mQueueFamilyIdx = queueFamilyIdx;
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
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION;
        FASTCG_CHECK_VK_RESULT(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));
    }

    void VulkanGraphicsSystem::AcquirePhysicalDeviceProperties()
    {
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);

        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);

        for (VkFormat format = VK_FORMAT_UNDEFINED; format < LAST_FORMAT; format = (VkFormat)(((size_t)format) + 1))
        {
            vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &mPhysicalDeviceFormatProperties[format]);
        }

        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr);

        mPhysicalDeviceExtensionProperties.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(mPhysicalDevice, NULL, &extensionCount,
                                             &mPhysicalDeviceExtensionProperties[0]);

#if _DEBUG
        FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "Available device extensions:");
        for (const auto &rExtensionProperty : mPhysicalDeviceExtensionProperties)
        {
            FASTCG_LOG_DEBUG(VulkanGraphicsSystem, "- %s", rExtensionProperty.extensionName);
        }
#endif
    }

    void VulkanGraphicsSystem::AcquirePhysicalDeviceSurfaceProperties()
    {
        FASTCG_CHECK_VK_RESULT(
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &mSurfaceCapabilities));

        if (mArgs.rScreenWidth < mSurfaceCapabilities.minImageExtent.width ||
            mArgs.rScreenWidth > mSurfaceCapabilities.maxImageExtent.width)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Invalid screen width");
        }

        if (mArgs.rScreenHeight < mSurfaceCapabilities.minImageExtent.height ||
            mArgs.rScreenHeight > mSurfaceCapabilities.maxImageExtent.height)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Invalid screen height");
        }

        mMaxSimultaneousFrames = std::max(std::min(mArgs.maxSimultaneousFrames, mSurfaceCapabilities.maxImageCount),
                                          mSurfaceCapabilities.minImageCount);

        // FIXME: because of headless-mode, forcing max simultaneous frame to three so we don't need to write complex
        // logic to recreate objects that depend on the number of simultaneous frames (eg, fences).
        assert(mMaxSimultaneousFrames >= 3);
        mMaxSimultaneousFrames = 3;

        uint32_t surfaceFormatsCount = 0;
        FASTCG_CHECK_VK_RESULT(
            vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatsCount, nullptr));

        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
        FASTCG_CHECK_VK_RESULT(
            vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatsCount, &surfaceFormats[0]));

        const auto acceptableSurfaceFormats = std::vector<VkFormat>{
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_FORMAT_R8G8B8A8_UNORM,
        };
        const auto *pFirstSurfaceFormat = &surfaceFormats[0];
        const auto *pLastSurfaceFormat = pFirstSurfaceFormat + surfaceFormatsCount;
        auto it = std::find_if(pFirstSurfaceFormat, pLastSurfaceFormat,
                               [&acceptableSurfaceFormats](const auto &rSurfaceFormat) {
                                   return std::any_of(acceptableSurfaceFormats.begin(), acceptableSurfaceFormats.end(),
                                                      [&rSurfaceFormat](const auto &rAcceptableSurfaceFormat) {
                                                          return rSurfaceFormat.format == rAcceptableSurfaceFormat;
                                                      });
                               });
        if (it == pLastSurfaceFormat)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't find required surface format");
        }

        mSurfaceSwapChainFormat = *it;

        {
            uint32_t presentModesCount;
            FASTCG_CHECK_VK_RESULT(
                vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModesCount, nullptr));
            if (presentModesCount == 0)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: No present mode found");
            }
            std::vector<VkPresentModeKHR> presentModes(presentModesCount);
            FASTCG_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface,
                                                                             &presentModesCount, &presentModes[0]));
            auto it = std::find_if(presentModes.begin(), presentModes.end(),
                                   [](const auto &presentMode) { return presentMode == VK_PRESENT_MODE_MAILBOX_KHR; });
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

        VkDeviceQueueCreateInfo deviceQueueCreateInfo;
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pNext = nullptr;
        deviceQueueCreateInfo.flags = 0;
        deviceQueueCreateInfo.queueFamilyIndex = mQueueFamilyIdx;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = sc_queuePriorities;

        if (!Contains(mPhysicalDeviceExtensionProperties, "VK_KHR_swapchain"))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't find VK_KHR_swapchain device extension");
        }
        mPhysicalDeviceExtensions.push_back("VK_KHR_swapchain");

        if (!Contains(mPhysicalDeviceExtensionProperties, "VK_KHR_create_renderpass2"))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't find VK_KHR_create_renderpass2 device extension");
        }
        mPhysicalDeviceExtensions.push_back("VK_KHR_create_renderpass2");

        VkDeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = nullptr;
        deviceCreateInfo.flags = 0;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = (uint32_t)mPhysicalDeviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames =
            mPhysicalDeviceExtensions.empty() ? nullptr : &mPhysicalDeviceExtensions[0];
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
        deviceCreateInfo.pEnabledFeatures = nullptr;

        FASTCG_CHECK_VK_RESULT(
            vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, mAllocationCallbacks.get(), &mDevice));

        LoadVulkanDeviceExtensionFunctions(mDevice);

        vkGetDeviceQueue(mDevice, mQueueFamilyIdx, 0, &mQueue);
    }

    void VulkanGraphicsSystem::RecreateSwapChain()
    {
        DestroySwapChain();

        if (mSurface == VK_NULL_HANDLE)
        {
            CreateSurfacelessSwapChain();
        }
        else
        {
            CreateSurfaceSwapChainAndAcquireNextImage();
        }
    }

    void VulkanGraphicsSystem::CreateSurfaceSwapChainAndAcquireNextImage()
    {
        VkCompositeAlphaFlagBitsKHR compositeAlpha;
        if ((mSurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0)
        {
            compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }
        else
        {
            assert((mSurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != 0);
            compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        }

        VkSwapchainCreateInfoKHR swapChainCreateInfo;
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.pNext = nullptr;
        swapChainCreateInfo.flags = 0;
        swapChainCreateInfo.surface = mSurface;
        swapChainCreateInfo.minImageCount = mMaxSimultaneousFrames;
        swapChainCreateInfo.imageFormat = mSurfaceSwapChainFormat.format;
        swapChainCreateInfo.imageColorSpace = mSurfaceSwapChainFormat.colorSpace;
        swapChainCreateInfo.imageExtent = {mArgs.rScreenWidth, mArgs.rScreenHeight};
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
#if defined FASTCG_ANDROID
        // let the presentation engine transform the image if necessary
        swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
#else
        swapChainCreateInfo.preTransform = mSurfaceCapabilities.currentTransform;
#endif
        swapChainCreateInfo.compositeAlpha = compositeAlpha;
        swapChainCreateInfo.presentMode = mPresentMode;
        swapChainCreateInfo.clipped = VK_TRUE;
        swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        FASTCG_CHECK_VK_RESULT(
            vkCreateSwapchainKHR(mDevice, &swapChainCreateInfo, mAllocationCallbacks.get(), &mSurfaceSwapChain));

        uint32_t swapChainCount;
        FASTCG_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(mDevice, mSurfaceSwapChain, &swapChainCount, nullptr));
        std::vector<VkImage> swapChainImages;
        swapChainImages.resize(swapChainCount);
        FASTCG_CHECK_VK_RESULT(
            vkGetSwapchainImagesKHR(mDevice, mSurfaceSwapChain, &swapChainCount, &swapChainImages[0]));

        VulkanTexture::Args args{"",
                                 mArgs.rScreenWidth,
                                 mArgs.rScreenHeight,
                                 1,
                                 1,
                                 TextureType::TEXTURE_2D,
                                 TextureUsageFlagBit::PRESENT,
                                 GetTextureFormat(mSurfaceSwapChainFormat.format),
                                 TextureFilter::LINEAR_FILTER,
                                 TextureWrapMode::CLAMP};
        for (size_t i = 0; i < swapChainImages.size(); ++i)
        {
            const auto &rSwapChainImage = swapChainImages[i];
            args.name = "Surface SwapChain Image " + std::to_string(i);
            args.image = rSwapChainImage;
            mSwapChainTextures.emplace_back(CreateTexture(args));
        }

        AcquireNextSurfaceSwapChainImage();
    }

    void VulkanGraphicsSystem::AcquireNextSurfaceSwapChainImage()
    {
        auto result = vkAcquireNextImageKHR(mDevice, mSurfaceSwapChain, UINT64_MAX,
                                            mAcquireSurfaceSwapChainImageSemaphores[mCurrentFrame], VK_NULL_HANDLE,
                                            &mSwapChainIndex);
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

    void VulkanGraphicsSystem::CreateSurfacelessSwapChain()
    {
        for (size_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            mSwapChainTextures.emplace_back(CreateTexture(
                {"Surfaceless SwapChain Image " + std::to_string(i), mArgs.rScreenWidth, mArgs.rScreenHeight, 1, 1,
                 TextureType::TEXTURE_2D, TextureUsageFlagBit::RENDER_TARGET | TextureUsageFlagBit::SAMPLED,
                 TextureFormat::R8G8B8A8_UNORM, TextureFilter::LINEAR_FILTER, TextureWrapMode::CLAMP}));
        }
    }

    void VulkanGraphicsSystem::CreateSynchronizationObjects()
    {
        mFrameFences.resize(mMaxSimultaneousFrames);
        mAcquireSurfaceSwapChainImageSemaphores.resize(mMaxSimultaneousFrames);
        mSubmitFinishedSemaphores.resize(mMaxSimultaneousFrames);

        VkFenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = nullptr;
        fenceCreateInfo.flags = 0;

        VkSemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;
        for (uint32_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            // create all fences that are not the current frame fence in signaled state
            fenceCreateInfo.flags = i != mCurrentFrame ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
            FASTCG_CHECK_VK_RESULT(
                vkCreateFence(mDevice, &fenceCreateInfo, mAllocationCallbacks.get(), &mFrameFences[i]));

            FASTCG_CHECK_VK_RESULT(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, mAllocationCallbacks.get(),
                                                     &mAcquireSurfaceSwapChainImageSemaphores[i]));
            FASTCG_CHECK_VK_RESULT(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, mAllocationCallbacks.get(),
                                                     &mSubmitFinishedSemaphores[i]));
        }
    }

    void VulkanGraphicsSystem::CreateCommandPoolAndCommandBuffers()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = mQueueFamilyIdx;
        FASTCG_CHECK_VK_RESULT(
            vkCreateCommandPool(mDevice, &commandPoolCreateInfo, mAllocationCallbacks.get(), &mCommandPool));

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
        const size_t MAX_LOCAL_POOL_COUNT = 16;
        const size_t DESCRIPTOR_TYPE_COUNT = 16;
        const VkDescriptorPoolSize DESCRIPTOR_POOL_SIZES[] = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             VulkanDescriptorSetLocalPool::MAX_SET_COUNT * DESCRIPTOR_TYPE_COUNT},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VulkanDescriptorSetLocalPool::MAX_SET_COUNT * DESCRIPTOR_TYPE_COUNT},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanDescriptorSetLocalPool::MAX_SET_COUNT * DESCRIPTOR_TYPE_COUNT}};

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext = nullptr;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInfo.maxSets =
            VulkanDescriptorSetLocalPool::MAX_SET_COUNT * MAX_LOCAL_POOL_COUNT * mMaxSimultaneousFrames;
        descriptorPoolCreateInfo.poolSizeCount = (uint32_t)FASTCG_ARRAYSIZE(DESCRIPTOR_POOL_SIZES);
        descriptorPoolCreateInfo.pPoolSizes = DESCRIPTOR_POOL_SIZES;
        FASTCG_CHECK_VK_RESULT(
            vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, mAllocationCallbacks.get(), &mDescriptorPool));

        mDescriptorSetLocalPools.resize(mMaxSimultaneousFrames);
    }

    void VulkanGraphicsSystem::CreateQueryPool()
    {
        // TODO: make this less brittle and possibly dynamic
        VkQueryPoolCreateInfo queryPoolInfo;
        queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolInfo.pNext = nullptr;
        queryPoolInfo.flags = 0;
        queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        queryPoolInfo.queryCount = 128;
        queryPoolInfo.pipelineStatistics = 0;

        mQueryPools.resize(mMaxSimultaneousFrames);
        mNextQueries.resize(mMaxSimultaneousFrames);
        for (uint32_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            FASTCG_CHECK_VK_RESULT(vkCreateQueryPool(mDevice, &queryPoolInfo, nullptr, &mQueryPools[i]));
            mNextQueries[i] = 0;
        }
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

    void VulkanGraphicsSystem::ResetQueryPool()
    {
        // TODO: make this less brittle and possibly dynamic
        vkCmdResetQueryPool(mCommandBuffers[mCurrentFrame], mQueryPools[mCurrentFrame], 0, 128);
        mNextQueries[mCurrentFrame] = 0;
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
        std::for_each(mFrameBuffers.begin(), mFrameBuffers.end(), [&](const auto &rEntry) {
            vkDestroyFramebuffer(mDevice, rEntry.second, mAllocationCallbacks.get());
        });
        mFrameBuffers.clear();
        mRenderTargetToFrameBufferHash.clear();
    }

    void VulkanGraphicsSystem::DestroyRenderPasses()
    {
        std::for_each(mRenderPasses.begin(), mRenderPasses.end(), [&](const auto &rEntry) {
            vkDestroyRenderPass(mDevice, rEntry.second, mAllocationCallbacks.get());
        });
        mRenderPasses.clear();
    }

    void VulkanGraphicsSystem::DestroyPipelineLayouts()
    {
        std::for_each(mPipelineLayouts.begin(), mPipelineLayouts.end(), [&](const auto &rEntry) {
            vkDestroyPipelineLayout(mDevice, rEntry.second, mAllocationCallbacks.get());
        });
        mPipelineLayouts.clear();
    }

    void VulkanGraphicsSystem::DestroyPipelines()
    {
        std::for_each(mPipelines.begin(), mPipelines.end(), [&](const auto &rEntry) {
            vkDestroyPipeline(mDevice, rEntry.second, mAllocationCallbacks.get());
        });
        mPipelines.clear();
    }

    void VulkanGraphicsSystem::DestroyDescriptorSetLayouts()
    {
        std::for_each(mDescriptorSetLayouts.begin(), mDescriptorSetLayouts.end(), [&](const auto &rEntry) {
            vkDestroyDescriptorSetLayout(mDevice, rEntry.second, mAllocationCallbacks.get());
        });
        mDescriptorSetLayouts.clear();
    }

    void VulkanGraphicsSystem::DestroyQueryPool()
    {
        for (uint32_t i = 0; i < mMaxSimultaneousFrames; ++i)
        {
            vkDestroyQueryPool(mDevice, mQueryPools[i], nullptr);
        }
        mQueryPools.clear();
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
            vkDestroySemaphore(mDevice, mAcquireSurfaceSwapChainImageSemaphores[i], mAllocationCallbacks.get());
            vkDestroySemaphore(mDevice, mSubmitFinishedSemaphores[i], mAllocationCallbacks.get());
            vkDestroyFence(mDevice, mFrameFences[i], mAllocationCallbacks.get());
        }
        mAcquireSurfaceSwapChainImageSemaphores.clear();
        mSubmitFinishedSemaphores.clear();
        mFrameFences.clear();
    }

    void VulkanGraphicsSystem::DestroySwapChain()
    {
        if (mSurfaceSwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(mDevice, mSurfaceSwapChain, mAllocationCallbacks.get());
            mSurfaceSwapChain = VK_NULL_HANDLE;
        }
        for (const auto &pSwapChainTexture : mSwapChainTextures)
        {
            DestroyTexture(pSwapChainTexture);
        }
        mSwapChainTextures.clear();
        mSwapChainIndex = 0;
    }

    void VulkanGraphicsSystem::DestroyDeviceAndClearQueues()
    {
        if (mDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(mDevice, mAllocationCallbacks.get());
            mDevice = VK_NULL_HANDLE;
        }
        mQueue = VK_NULL_HANDLE;
        mQueueFamilyIdx = ~0u;
        mQueueSupportsCompute = false;
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
            if (Contains(mInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                VkExt::vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
            }
#endif

            vkDestroyInstance(mInstance, mAllocationCallbacks.get());
            mInstance = VK_NULL_HANDLE;
        }
    }

    void VulkanGraphicsSystem::SwapFrame()
    {
        if (mSurface != VK_NULL_HANDLE)
        {
            auto imageMemoryBarrier = GetLastImageMemoryBarrier(GetCurrentSwapChainTexture());
            if (imageMemoryBarrier.layout != VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
            {
                GetImmediateGraphicsContext()->AddTextureMemoryBarrier(
                    GetCurrentSwapChainTexture(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, imageMemoryBarrier.accessMask, 0,
                    imageMemoryBarrier.stageMask, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
            }
        }

        GetImmediateGraphicsContext()->End();

        EndCurrentCommandBuffer();

        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        if (mSurface == VK_NULL_HANDLE)
        {
            submitInfo.waitSemaphoreCount = 0;
            submitInfo.pWaitSemaphores = nullptr;
            submitInfo.pWaitDstStageMask = nullptr;
        }
        else
        {
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &mAcquireSurfaceSwapChainImageSemaphores[mCurrentFrame];
            submitInfo.pWaitDstStageMask = &waitDstStageMask;
        }
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
        if (mSurface == VK_NULL_HANDLE)
        {
            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores = nullptr;
        }
        else
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &mSubmitFinishedSemaphores[mCurrentFrame];
        }
        if (vkQueueSubmit(mQueue, 1, &submitInfo, mFrameFences[mCurrentFrame]) != VK_SUCCESS)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't submit commands");
        }

        bool outdatedSwapChain = false;
        if (mSurface != VK_NULL_HANDLE)
        {
            VkPresentInfoKHR presentInfo;
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.pNext = nullptr;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &mSubmitFinishedSemaphores[mCurrentFrame];
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &mSurfaceSwapChain;
            presentInfo.pImageIndices = &mSwapChainIndex;
            presentInfo.pResults = nullptr;

            auto result = vkQueuePresentKHR(mQueue, &presentInfo);
            switch (result)
            {
            case VK_SUCCESS:
            case VK_SUBOPTIMAL_KHR:
                break;
            case VK_ERROR_OUT_OF_DATE_KHR:
                outdatedSwapChain = true;
                break;
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't present");
                break;
            }
        }

        mCurrentFrame = (mCurrentFrame + 1) % mMaxSimultaneousFrames;

        FASTCG_CHECK_VK_RESULT(vkWaitForFences(mDevice, 1, &mFrameFences[mCurrentFrame], VK_TRUE, UINT64_MAX));
        FASTCG_CHECK_VK_RESULT(vkResetFences(mDevice, 1, &mFrameFences[mCurrentFrame]));

#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime(mCurrentFrame);
        }
#endif

        PerformDeferredDestroys();

        for (auto &rEntry : mDescriptorSetLocalPools[mCurrentFrame])
        {
            rEntry.second.lastDescriptorSetIdx = 0;
        }

        BeginCurrentCommandBuffer();

        ResetQueryPool();

        GetImmediateGraphicsContext()->Begin();

        if (mSurface != VK_NULL_HANDLE)
        {
            if (outdatedSwapChain)
            {
                RecreateSwapChain();
            }
            else
            {
                AcquireNextSurfaceSwapChainImage();
            }
        }
        else
        {
            mSwapChainIndex = (mSwapChainIndex + 1) % mMaxSimultaneousFrames;
        }
    }

    double VulkanGraphicsSystem::GetGpuElapsedTime(uint32_t frame) const
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        double elapsedTime = 0;
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            elapsedTime += pGraphicsContext->GetElapsedTime(frame);
        }
        return elapsedTime;
#else
        return 0;
#endif
    }

    std::pair<size_t, VkRenderPass> VulkanGraphicsSystem::GetOrCreateRenderPass(
        const VulkanRenderPassDescription &rRenderPassDescription, bool depthWrite, bool stencilWrite)
    {
        std::vector<AttachmentDefinition> attachmentDefinitions;
        for (size_t i = 0; i < rRenderPassDescription.renderTargetCount; ++i)
        {
            const auto *pRenderTarget = rRenderPassDescription.ppRenderTargets[i];
            if (pRenderTarget == nullptr)
            {
                continue;
            }

            attachmentDefinitions.emplace_back();
            auto &rAttachmentDefinition = attachmentDefinitions.back();
            rAttachmentDefinition.pTexture = pRenderTarget;

            if ((rRenderPassDescription.colorClearRequests[i].flags & VulkanClearRequestFlagBit::COLOR_OR_DEPTH) != 0)
            {
                rAttachmentDefinition.colorOrDepth = VK_ATTACHMENT_LOAD_OP_CLEAR;
            }
            else
            {
                rAttachmentDefinition.colorOrDepth = VK_ATTACHMENT_LOAD_OP_LOAD;
            }
            rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            rAttachmentDefinition.write = 1; // write color
        }

        if (rRenderPassDescription.pDepthStencilBuffer != nullptr)
        {
            attachmentDefinitions.emplace_back();
            auto &rAttachmentDefinition = attachmentDefinitions.back();
            rAttachmentDefinition.pTexture = rRenderPassDescription.pDepthStencilBuffer;

            if ((rRenderPassDescription.depthStencilClearRequest.flags & VulkanClearRequestFlagBit::COLOR_OR_DEPTH) !=
                0)
            {
                rAttachmentDefinition.colorOrDepth = VK_ATTACHMENT_LOAD_OP_CLEAR;
            }
            else
            {
                rAttachmentDefinition.stencil = VK_ATTACHMENT_LOAD_OP_LOAD;
            }

            if (HasStencil(rRenderPassDescription.pDepthStencilBuffer->GetFormat()))
            {
                if ((rRenderPassDescription.depthStencilClearRequest.flags & VulkanClearRequestFlagBit::STENCIL) != 0)
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
            rAttachmentDefinition.write =
                (depthWrite) + (stencilWrite << 1); // no write, write depth or write depth&stencil
        }

        auto renderPassHash = GetRenderPassHash(attachmentDefinitions);
        auto it = mRenderPasses.find(renderPassHash);
        if (it != mRenderPasses.end())
        {
            return {it->first, it->second};
        }

        VkRenderPassCreateInfo2KHR renderPassCreateInfo;
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
        renderPassCreateInfo.pNext = nullptr;
        renderPassCreateInfo.flags = 0;
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = nullptr;
        renderPassCreateInfo.correlatedViewMaskCount = 0;
        renderPassCreateInfo.pCorrelatedViewMasks = nullptr;

        VkSubpassDescription2KHR subpassDescription;
        subpassDescription.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
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

        std::vector<VkAttachmentDescription2KHR> attachmentDescriptions;
        std::vector<VkAttachmentReference2KHR> colorAttachmentReferences;

        subpassDescription.pDepthStencilAttachment = nullptr;

        for (uint32_t i = 0; i < rRenderPassDescription.renderTargetCount; ++i)
        {
            const auto *pRenderTarget = rRenderPassDescription.ppRenderTargets[i];
            if (pRenderTarget == nullptr)
            {
                continue;
            }

            auto attachmentIdx = (uint32_t)attachmentDescriptions.size();

            auto &rAttachmentDefinition = attachmentDefinitions[attachmentIdx];

            attachmentDescriptions.emplace_back();
            auto &rAttachmentDescription = attachmentDescriptions.back();
            rAttachmentDescription.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
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
            rColorAttachmentReference.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
            rColorAttachmentReference.pNext = nullptr;
            rColorAttachmentReference.attachment = attachmentIdx;
            rColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            rColorAttachmentReference.aspectMask = pRenderTarget->GetAspectFlags();
        }

        VkAttachmentReference2KHR depthAttachmentReference;
        if (rRenderPassDescription.pDepthStencilBuffer != nullptr)
        {
            auto attachmentIdx = (uint32_t)attachmentDescriptions.size();

            auto &rAttachmentDefinition = attachmentDefinitions[attachmentIdx];

            attachmentDescriptions.emplace_back();
            auto &rAttachmentDescription = attachmentDescriptions.back();
            rAttachmentDescription.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
            rAttachmentDescription.pNext = nullptr;
            rAttachmentDescription.flags = 0;
            rAttachmentDescription.format = rRenderPassDescription.pDepthStencilBuffer->GetVulkanFormat();
            rAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            rAttachmentDescription.loadOp = rAttachmentDefinition.colorOrDepth;
            rAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkImageLayout initialLayout, finalLayout;
            if (depthWrite || stencilWrite)
            {
                initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                finalLayout = rRenderPassDescription.pDepthStencilBuffer->GetRestingLayout();
            }
            else
            {
                initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            }

            depthAttachmentReference.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
            depthAttachmentReference.pNext = nullptr;
            depthAttachmentReference.attachment = attachmentIdx;
            depthAttachmentReference.layout = initialLayout;
            depthAttachmentReference.aspectMask = rRenderPassDescription.pDepthStencilBuffer->GetAspectFlags();

            rAttachmentDescription.stencilLoadOp = rAttachmentDefinition.stencil;
            if (stencilWrite)
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
        subpassDescription.pColorAttachments =
            colorAttachmentReferences.empty() ? nullptr : &colorAttachmentReferences[0];

        VkRenderPass renderPass;
        FASTCG_CHECK_VK_RESULT(
            VkExt::vkCreateRenderPass2KHR(mDevice, &renderPassCreateInfo, mAllocationCallbacks.get(), &renderPass));

        it = mRenderPasses.emplace(renderPassHash, renderPass).first;

        return {it->first, it->second};
    }

    std::pair<size_t, VkFramebuffer> VulkanGraphicsSystem::GetOrCreateFrameBuffer(
        const VulkanRenderPassDescription &rRenderPassDescription, bool depthWrite, bool stencilWrite)
    {
        auto result = GetOrCreateRenderPass(rRenderPassDescription, depthWrite, stencilWrite);

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
        auto CalculateFramebufferDimensions = [&framebufferCreateInfo](const VulkanTexture *pRenderTarget) {
            // according to VUID-VkFramebufferCreateInfo-flags-04533/0433, each element of pAttachments (...)
            // must have been created with a (...) width/height greater than or equal the width/height of the
            // framebuffer
            framebufferCreateInfo.width = std::min(framebufferCreateInfo.width, pRenderTarget->GetWidth());
            framebufferCreateInfo.height = std::min(framebufferCreateInfo.height, pRenderTarget->GetHeight());
        };

        for (uint32_t i = 0; i < rRenderPassDescription.renderTargetCount; ++i)
        {
            const auto *pRenderTarget = rRenderPassDescription.ppRenderTargets[i];
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
        FASTCG_CHECK_VK_RESULT(
            vkCreateFramebuffer(mDevice, &framebufferCreateInfo, mAllocationCallbacks.get(), &frameBuffer));

        it = mFrameBuffers.emplace(result.first, frameBuffer).first;

        for (uint32_t i = 0; i < rRenderPassDescription.renderTargetCount; ++i)
        {
            const auto *pRenderTarget = rRenderPassDescription.ppRenderTargets[i];
            if (pRenderTarget != nullptr)
            {
                auto image = pRenderTarget->GetImage();
                mRenderTargetToFrameBufferHash[image] = result.first;
                mFrameBufferHashToRenderTargets[result.first].emplace_back(image);
            }
        }

        return {it->first, it->second};
    }

    std::pair<size_t, VulkanPipeline> VulkanGraphicsSystem::GetOrCreateGraphicsPipeline(
        const VulkanPipelineDescription &rPipelineDescription, VkRenderPass renderPass, uint32_t renderTargetCount)
    {
        assert(rPipelineDescription.pShader != nullptr);

        auto pipelineLayout =
            GetOrCreatePipelineLayout(rPipelineDescription.pShader->GetPipelineLayoutDescription()).second;

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

        const auto &rVertexInputDescription = rPipelineDescription.pShader->GetInputDescription();
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
        std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
        for (uint32_t i = 0; i < rPipelineDescription.graphicsInfo.vertexBufferCount; ++i)
        {
            const auto *pVertexBuffer = rPipelineDescription.graphicsInfo.ppVertexBuffers[i];
            uint32_t stride = 0;
            for (const auto &rVbDesc : pVertexBuffer->GetVertexBindingDescriptors())
            {
                auto it = rVertexInputDescription.find(rVbDesc.binding);
                if (it == rVertexInputDescription.end())
                {
                    continue;
                }
                auto format = GetVkFormat(rVbDesc.type, rVbDesc.size);
                vertexInputAttributeDescriptions.emplace_back(
                    VkVertexInputAttributeDescription{rVbDesc.binding, (uint32_t)i, format, rVbDesc.offset});
                stride += GetVkStride(format);
            }
            vertexInputBindingDescriptions.emplace_back(
                VkVertexInputBindingDescription{(uint32_t)i, stride, VK_VERTEX_INPUT_RATE_VERTEX});
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
        rasterizationStateCreateInfo.cullMode = GetVkCullModeFlags(rPipelineDescription.graphicsInfo.cullMode);
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
        depthStencilStateCreateInfo.depthTestEnable = rPipelineDescription.graphicsInfo.depthTest;
        depthStencilStateCreateInfo.depthWriteEnable = rPipelineDescription.graphicsInfo.depthWrite;
        depthStencilStateCreateInfo.depthCompareOp = GetVkCompareOp(rPipelineDescription.graphicsInfo.depthFunc);
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.stencilTestEnable = rPipelineDescription.graphicsInfo.stencilTest;
        depthStencilStateCreateInfo.front.compareMask = rPipelineDescription.graphicsInfo.stencilFrontState.compareMask;
        depthStencilStateCreateInfo.front.compareOp =
            GetVkCompareOp(rPipelineDescription.graphicsInfo.stencilFrontState.compareOp);
        depthStencilStateCreateInfo.front.depthFailOp =
            GetVkStencilOp(rPipelineDescription.graphicsInfo.stencilFrontState.depthFailOp);
        depthStencilStateCreateInfo.front.failOp =
            GetVkStencilOp(rPipelineDescription.graphicsInfo.stencilFrontState.stencilFailOp);
        depthStencilStateCreateInfo.front.passOp =
            GetVkStencilOp(rPipelineDescription.graphicsInfo.stencilFrontState.passOp);
        depthStencilStateCreateInfo.front.reference = rPipelineDescription.graphicsInfo.stencilFrontState.reference;
        depthStencilStateCreateInfo.front.writeMask = rPipelineDescription.graphicsInfo.stencilFrontState.writeMask;
        depthStencilStateCreateInfo.back.compareMask = rPipelineDescription.graphicsInfo.stencilBackState.compareMask;
        depthStencilStateCreateInfo.back.compareOp =
            GetVkCompareOp(rPipelineDescription.graphicsInfo.stencilBackState.compareOp);
        depthStencilStateCreateInfo.back.depthFailOp =
            GetVkStencilOp(rPipelineDescription.graphicsInfo.stencilBackState.depthFailOp);
        depthStencilStateCreateInfo.back.failOp =
            GetVkStencilOp(rPipelineDescription.graphicsInfo.stencilBackState.stencilFailOp);
        depthStencilStateCreateInfo.back.passOp =
            GetVkStencilOp(rPipelineDescription.graphicsInfo.stencilBackState.passOp);
        depthStencilStateCreateInfo.back.reference = rPipelineDescription.graphicsInfo.stencilBackState.reference;
        depthStencilStateCreateInfo.back.writeMask = rPipelineDescription.graphicsInfo.stencilBackState.writeMask;
        depthStencilStateCreateInfo.minDepthBounds = 0;
        depthStencilStateCreateInfo.maxDepthBounds = 1;
        pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        colorBlendAttachmentStates.resize(renderTargetCount);
        for (auto &rColorBlendAttachmentState : colorBlendAttachmentStates)
        {
            rColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            rColorBlendAttachmentState.blendEnable = rPipelineDescription.graphicsInfo.blend;
            rColorBlendAttachmentState.colorBlendOp =
                GetVkBlendOp(rPipelineDescription.graphicsInfo.blendState.colorOp);
            rColorBlendAttachmentState.srcColorBlendFactor =
                GetVkBlendFactor(rPipelineDescription.graphicsInfo.blendState.srcColorFactor);
            rColorBlendAttachmentState.dstColorBlendFactor =
                GetVkBlendFactor(rPipelineDescription.graphicsInfo.blendState.dstColorFactor);
            rColorBlendAttachmentState.alphaBlendOp =
                GetVkBlendOp(rPipelineDescription.graphicsInfo.blendState.alphaOp);
            rColorBlendAttachmentState.srcAlphaBlendFactor =
                GetVkBlendFactor(rPipelineDescription.graphicsInfo.blendState.srcAlphaFactor);
            rColorBlendAttachmentState.dstAlphaBlendFactor =
                GetVkBlendFactor(rPipelineDescription.graphicsInfo.blendState.dstAlphaFactor);
        }

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.pNext = nullptr;
        colorBlendStateCreateInfo.flags = 0;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
        colorBlendStateCreateInfo.attachmentCount = (uint32_t)colorBlendAttachmentStates.size();
        colorBlendStateCreateInfo.pAttachments =
            colorBlendAttachmentStates.empty() ? nullptr : &colorBlendAttachmentStates[0];
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
        FASTCG_CHECK_VK_RESULT(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo,
                                                         mAllocationCallbacks.get(), &pipeline));

        it = mPipelines.emplace(pipelineHash, pipeline).first;

        return {it->first, {it->second, pipelineLayout}};
    }

    std::pair<size_t, VulkanPipeline> VulkanGraphicsSystem::GetOrCreateComputePipeline(
        const VulkanPipelineDescription &rPipelineDescription)
    {
        assert(rPipelineDescription.pShader != nullptr);

        auto pipelineLayout =
            GetOrCreatePipelineLayout(rPipelineDescription.pShader->GetPipelineLayoutDescription()).second;

        auto pipelineHash = GetPipelineHash(rPipelineDescription);

        auto it = mPipelines.find(pipelineHash);
        if (it != mPipelines.end())
        {
            return {it->first, {it->second, pipelineLayout}};
        }

        VkComputePipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = nullptr;
        pipelineCreateInfo.flags = 0;
        pipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineCreateInfo.stage.pNext = nullptr;
        pipelineCreateInfo.stage.flags = 0;
        pipelineCreateInfo.stage.pName = "main";
        pipelineCreateInfo.stage.module = rPipelineDescription.pShader->GetModule(ShaderType::COMPUTE);
        pipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipelineCreateInfo.stage.pSpecializationInfo = NULL;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = 0;

        VkPipeline pipeline;
        FASTCG_CHECK_VK_RESULT(vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo,
                                                        mAllocationCallbacks.get(), &pipeline));

        it = mPipelines.emplace(pipelineHash, pipeline).first;

        return {it->first, {it->second, pipelineLayout}};
    }

    std::pair<size_t, VkPipelineLayout> VulkanGraphicsSystem::GetOrCreatePipelineLayout(
        const VulkanPipelineLayoutDescription &rPipelineLayoutDescription)
    {
        auto pipelineLayoutHash = GetPipelineLayoutHash(rPipelineLayoutDescription);

        auto it = mPipelineLayouts.find(pipelineLayoutHash);
        if (it != mPipelineLayouts.end())
        {
            return {it->first, it->second};
        }

        VkDescriptorSetLayout pSetLayouts[VulkanPipelineLayout::MAX_SET_COUNT];
        for (uint32_t i = 0; i < rPipelineLayoutDescription.setLayoutCount; ++i)
        {
            const auto &rSetLayout = rPipelineLayoutDescription.pSetLayouts[i];
            if (rSetLayout.bindingLayoutCount > 0)
            {
                pSetLayouts[i] = GetOrCreateDescriptorSetLayout(rSetLayout).second;
            }
            else
            {
                pSetLayouts[i] = nullptr;
            }
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = nullptr;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = rPipelineLayoutDescription.setLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        VkPipelineLayout pipelineLayout;
        FASTCG_CHECK_VK_RESULT(
            vkCreatePipelineLayout(VulkanGraphicsSystem::GetInstance()->GetDevice(), &pipelineLayoutCreateInfo,
                                   VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(), &pipelineLayout));

        it = mPipelineLayouts.emplace(pipelineLayoutHash, pipelineLayout).first;

        return {it->first, it->second};
    }

    std::pair<size_t, VkDescriptorSetLayout> VulkanGraphicsSystem::GetOrCreateDescriptorSetLayout(
        const VulkanDescriptorSetLayout &rDescriptorSetLayout)
    {
        assert(rDescriptorSetLayout.bindingLayoutCount > 0);

        auto setLayoutHash = GetDescriptorSetLayoutHash(rDescriptorSetLayout);

        auto it = mDescriptorSetLayouts.find(setLayoutHash);
        if (it != mDescriptorSetLayouts.end())
        {
            return {it->first, it->second};
        }

        VkDescriptorSetLayoutBinding pLayoutBindings[VulkanDescriptorSet::MAX_BINDING_COUNT];
        for (uint32_t i = 0; i < rDescriptorSetLayout.bindingLayoutCount; ++i)
        {
            const auto &rBinding = rDescriptorSetLayout.pBindingLayouts[i];
            auto &rLayoutBinding = pLayoutBindings[i];
            rLayoutBinding.binding = rBinding.binding;
            rLayoutBinding.descriptorCount = 1;
            rLayoutBinding.descriptorType = rBinding.type;
            rLayoutBinding.pImmutableSamplers = nullptr;
            rLayoutBinding.stageFlags = rBinding.stageFlags;
        }

        VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo;
        setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutCreateInfo.pNext = nullptr;
        setLayoutCreateInfo.flags = 0;
        setLayoutCreateInfo.bindingCount = rDescriptorSetLayout.bindingLayoutCount;
        setLayoutCreateInfo.pBindings = pLayoutBindings;

        VkDescriptorSetLayout setLayout;
        FASTCG_CHECK_VK_RESULT(
            vkCreateDescriptorSetLayout(VulkanGraphicsSystem::GetInstance()->GetDevice(), &setLayoutCreateInfo,
                                        VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(), &setLayout));

        it = mDescriptorSetLayouts.emplace(setLayoutHash, setLayout).first;

        return {it->first, it->second};
    }

    std::pair<size_t, VkDescriptorSet> VulkanGraphicsSystem::GetOrCreateDescriptorSet(
        const VulkanDescriptorSetLayout &rDescriptorSetLayout)
    {
        auto setLayoutHash = GetDescriptorSetLayoutHash(rDescriptorSetLayout);

        auto &rDescriptorSetLocalPools = mDescriptorSetLocalPools[mCurrentFrame];
        auto it = rDescriptorSetLocalPools.find(setLayoutHash);
        if (it != rDescriptorSetLocalPools.end())
        {
            auto &rDescriptorSetLocalPool = it->second;
            auto descriptorSet = rDescriptorSetLocalPool.descriptorSets[rDescriptorSetLocalPool.lastDescriptorSetIdx++];
            assert(rDescriptorSetLocalPool.lastDescriptorSetIdx < VulkanDescriptorSetLocalPool::MAX_SET_COUNT);
            return {it->first, descriptorSet};
        }

        auto setLayout = GetOrCreateDescriptorSetLayout(rDescriptorSetLayout).second;

        auto &rDescriptorSetLocalPool = rDescriptorSetLocalPools[setLayoutHash];

        for (size_t i = 0; i < VulkanDescriptorSetLocalPool::MAX_SET_COUNT; ++i)
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
        assert(rDescriptorSetLocalPool.lastDescriptorSetIdx < VulkanDescriptorSetLocalPool::MAX_SET_COUNT);
        return {setLayoutHash, descriptorSet};
    }

#if _DEBUG
    void VulkanGraphicsSystem::PushDebugMarker(VkCommandBuffer commandBuffer, const char *pName)
    {
        if (!Contains(mInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            return;
        }

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
        if (!Contains(mInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            return;
        }

        VkExt::vkCmdEndDebugUtilsLabelEXT(commandBuffer);
    }

    void VulkanGraphicsSystem::SetObjectName(const char *pObjectName, VkObjectType objectType, uint64_t objectHandle)
    {
        if (!Contains(mInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            return;
        }

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
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled deferred destroy type %d",
                                       (int)rDeferredDestroyRequest.type);
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
                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled deferred destroy type %d",
                                       (int)rDeferredDestroyRequest.type);
                break;
            }
        }
        mDeferredDestroyRequests.clear();
    }

    void VulkanGraphicsSystem::OnPostWindowInitialize(void *pWindow)
    {
        CreateSurface(pWindow);
        AcquirePhysicalDeviceSurfaceProperties();
        RecreateSwapChain();
    }

    void VulkanGraphicsSystem::OnPreWindowTerminate(void *pWindow)
    {
        mSurfaceCapabilities = {};
        mPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        mSurfaceSwapChainFormat = {};
        DestroySwapChain();
        DestroySurface();
        CreateSurfacelessSwapChain();
    }

    void VulkanGraphicsSystem::Submit()
    {
        GetImmediateGraphicsContext()->End();

        EndCurrentCommandBuffer();

        VkFenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = nullptr;
        fenceCreateInfo.flags = 0;

        VkFence fence;
        FASTCG_CHECK_VK_RESULT(vkCreateFence(mDevice, &fenceCreateInfo, mAllocationCallbacks.get(), &fence));

        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;
        FASTCG_CHECK_VK_RESULT(vkQueueSubmit(mQueue, 1, &submitInfo, fence));

        FASTCG_CHECK_VK_RESULT(vkWaitForFences(mDevice, 1, &fence, VK_TRUE, UINT64_MAX));

        vkDestroyFence(mDevice, fence, mAllocationCallbacks.get());

#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime(mCurrentFrame);
        }
#endif

        PerformDeferredDestroys();

        for (auto &rEntry : mDescriptorSetLocalPools[mCurrentFrame])
        {
            rEntry.second.lastDescriptorSetIdx = 0;
        }

        BeginCurrentCommandBuffer();

        ResetQueryPool();

        GetImmediateGraphicsContext()->Begin();
    }

    void VulkanGraphicsSystem::WaitPreviousFrame()
    {
        auto previousFrame = GetPreviousFrame();
        FASTCG_CHECK_VK_RESULT(vkWaitForFences(mDevice, 1, &mFrameFences[previousFrame], VK_TRUE, UINT64_MAX));
        FASTCG_CHECK_VK_RESULT(vkResetFences(mDevice, 1, &mFrameFences[previousFrame]));
#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime(previousFrame);
        }
#endif
    }
}

#endif