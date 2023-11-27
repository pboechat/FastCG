#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>

#if defined FASTCG_LINUX
// use local spirv-cross on Linux
#include <spirv_cross/spirv_cross.hpp>
#else
#include <spirv_cross.hpp>
#endif

#include <memory>
#include <cstring>
#include <algorithm>

namespace
{
    inline VkFormat GetVkFormatFromSpirType(const spirv_cross::SPIRType &rType)
    {
        uint32_t componentCount = 0;
        bool isSigned = true;
        bool isFloatingPoint = false;

        switch (rType.basetype)
        {
        case spirv_cross::SPIRType::Float:
            isFloatingPoint = true;
            componentCount = rType.vecsize;
            break;
        case spirv_cross::SPIRType::Int:
            componentCount = rType.vecsize;
            break;
        case spirv_cross::SPIRType::UInt:
            isSigned = false;
            componentCount = rType.vecsize;
            break;
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: Unsupported SPIRType base type %d", rType.basetype);
            return (VkFormat)0;
        }

        switch (componentCount)
        {
        case 1:
            if (isFloatingPoint)
            {
                return VK_FORMAT_R32_SFLOAT;
            }
            else
            {
                if (isSigned)
                {
                    return VK_FORMAT_R32_SINT;
                }
                else
                {
                    return VK_FORMAT_R32_UINT;
                }
            }
            break;
        case 2:
            if (isFloatingPoint)
            {
                return VK_FORMAT_R32G32_SFLOAT;
            }
            else
            {
                if (isSigned)
                {
                    return VK_FORMAT_R32G32_SINT;
                }
                else
                {
                    return VK_FORMAT_R32G32_UINT;
                }
            }
            break;
        case 3:
            if (isFloatingPoint)
            {
                return VK_FORMAT_R32G32B32_SFLOAT;
            }
            else
            {
                if (isSigned)
                {
                    return VK_FORMAT_R32G32B32_SINT;
                }
                else
                {
                    return VK_FORMAT_R32G32B32_UINT;
                }
            }
            break;
        case 4:
            if (isFloatingPoint)
            {
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
            else
            {
                if (isSigned)
                {
                    return VK_FORMAT_R32G32B32A32_SINT;
                }
                else
                {
                    return VK_FORMAT_R32G32B32A32_UINT;
                }
            }
            break;
        }

        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: Don't know how to get VkFormat from SPIRType");
        return (VkFormat)0;
    }
}

namespace FastCG
{
    VulkanShader::VulkanShader(const Args &rArgs) : BaseShader(rArgs)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo;
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.pNext = nullptr;
        shaderModuleCreateInfo.flags = 0;

        auto AddResource = [&](const spirv_cross::Compiler &rCompiler, const spirv_cross::Resource &rResource, ShaderType shaderType, VkDescriptorType descriptorType)
        {
            auto set = rCompiler.get_decoration(rResource.id, spv::DecorationDescriptorSet);
            auto binding = rCompiler.get_decoration(rResource.id, spv::DecorationBinding);
            mResourceLocation[rResource.name] = {set, binding};
            if (mLayoutDescription.size() <= set)
            {
                mLayoutDescription.resize(set + 1);
            }
            auto &rDescriptorSet = mLayoutDescription[set];
            auto it = std::find_if(rDescriptorSet.begin(), rDescriptorSet.end(), [&binding](const auto &rBinding)
                                   { return rBinding.binding == binding; });
            if (it == rDescriptorSet.end())
            {
                it = rDescriptorSet.emplace(rDescriptorSet.end());
                it->binding = binding;
                it->type = descriptorType;
            }
            it->stageFlags |= GetVkShaderStageFlagBit(shaderType);
        };

        for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::LAST; ++i)
        {
            auto shaderType = (ShaderType)i;

            const auto &rProgramData = rArgs.programsData[i];
            if (rProgramData.dataSize == 0)
            {
                continue;
            }

            shaderModuleCreateInfo.codeSize = rProgramData.dataSize;
            auto wordCount = shaderModuleCreateInfo.codeSize / sizeof(uint32_t);
            auto pCode = std::make_unique<uint32_t[]>(wordCount);
            std::memcpy(pCode.get(), rProgramData.pData, shaderModuleCreateInfo.codeSize);
            shaderModuleCreateInfo.pCode = pCode.get();
            FASTCG_CHECK_VK_RESULT(vkCreateShaderModule(VulkanGraphicsSystem::GetInstance()->GetDevice(),
                                                        &shaderModuleCreateInfo,
                                                        VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(),
                                                        &mModules[i]));

#if _DEBUG
            VulkanGraphicsSystem::GetInstance()->SetObjectName((GetName() + " (" + GetShaderTypeString(shaderType) + ") (VkShaderModule)").c_str(), VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)mModules[i]);
#endif

            // TODO: cache reflection data
            spirv_cross::Compiler compiler(shaderModuleCreateInfo.pCode, wordCount);

            auto shaderResources = compiler.get_shader_resources();
            for (const auto &rBuffer : shaderResources.uniform_buffers)
            {
                AddResource(compiler, rBuffer, shaderType, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            }
            for (const auto &rBuffer : shaderResources.storage_buffers)
            {
                AddResource(compiler, rBuffer, shaderType, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
            }
            for (const auto &rTexture : shaderResources.sampled_images)
            {
                AddResource(compiler, rTexture, shaderType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            }

            if (shaderType == ShaderType::VERTEX)
            {
                for (const auto &rVertexInput : shaderResources.stage_inputs)
                {
                    mVertexInputDescription.emplace(compiler.get_decoration(rVertexInput.id, spv::DecorationLocation));
                }
            }
            else if (shaderType == ShaderType::FRAGMENT)
            {
                for (const auto &rFragmentOutput : shaderResources.stage_outputs)
                {
                    mFragmentOutputDescription.emplace(compiler.get_decoration(rFragmentOutput.id, spv::DecorationLocation));
                }
            }
        }
    }

    VulkanShader::~VulkanShader()
    {
        for (ShaderTypeInt i = 0; i < (ShaderTypeInt)ShaderType::LAST; ++i)
        {
            if (mModules[i] != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(VulkanGraphicsSystem::GetInstance()->GetDevice(),
                                      mModules[i],
                                      VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks());
                mModules[i] = VK_NULL_HANDLE;
            }
        }
    }

}

#endif