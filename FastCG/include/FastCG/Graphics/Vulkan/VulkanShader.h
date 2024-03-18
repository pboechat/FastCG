#ifndef FASTCG_VULKAN_SHADER_H
#define FASTCG_VULKAN_SHADER_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanDescriptorSet.h>
#include <FastCG/Graphics/BaseShader.h>
#include <FastCG/Core/Hash.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace FastCG
{
    class VulkanGraphicsContext;
    class VulkanGraphicsSystem;

    struct VulkanResourceLocation
    {
        uint32_t set{~0u};
        uint32_t binding{~0u};
    };

    using VulkanInputDescription = std::unordered_set<uint32_t, IdentityHasher<uint32_t>>;

    using VulkanOutputDescription = std::unordered_set<uint32_t, IdentityHasher<uint32_t>>;

    class VulkanShader : public BaseShader
    {
    public:
        VulkanShader(const Args &rArgs);
        VulkanShader(const VulkanShader &rOther) = delete;
        VulkanShader(const VulkanShader &&rOther) = delete;
        virtual ~VulkanShader();

        VulkanShader operator=(const VulkanShader &rOther) = delete;

        inline VulkanResourceLocation GetResourceLocation(const std::string &rName) const
        {
            auto it = mResourceLocation.find(rName);
            if (it != mResourceLocation.end())
            {
                return it->second;
            }
            else
            {
                return {};
            }
        }

    private:
        VkShaderModule mModules[(ShaderTypeInt)ShaderType::LAST]{};
        std::unordered_map<std::string, VulkanResourceLocation> mResourceLocation;
        VulkanPipelineLayoutDescription mPipelineLayoutDescription{};
        VulkanInputDescription mInputDescription;
        VulkanOutputDescription mOutputDescription;

        inline VkShaderModule GetModule(ShaderType shaderType) const
        {
            return mModules[(ShaderTypeInt)shaderType];
        }
        inline const VulkanPipelineLayoutDescription &GetPipelineLayoutDescription() const
        {
            return mPipelineLayoutDescription;
        }
        inline const VulkanInputDescription &GetInputDescription() const
        {
            return mInputDescription;
        }
        inline const VulkanOutputDescription &GetOutputDescription() const
        {
            return mOutputDescription;
        }

        friend class VulkanGraphicsContext;
        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif