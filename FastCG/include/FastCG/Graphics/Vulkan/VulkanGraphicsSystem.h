#ifndef FASTCG_VULKAN_GRAPHICS_SYSTEM_H
#define FASTCG_VULKAN_GRAPHICS_SYSTEM_H

#ifdef FASTCG_VULKAN

#include <FastCG/System.h>
#include <FastCG/ShaderConstants.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanMesh.h>
#include <FastCG/Graphics/Vulkan/VulkanMaterialDefinition.h>
#include <FastCG/Graphics/Vulkan/VulkanMaterial.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>
#include <algorithm>

namespace FastCG
{
    class VulkanGraphicsSystem : public BaseGraphicsSystem<VulkanBuffer, VulkanMaterial, VulkanMesh, VulkanGraphicsContext, VulkanShader, VulkanTexture>
    {
        FASTCG_DECLARE_SYSTEM(VulkanGraphicsSystem, GraphicsSystemArgs);

    public:
        inline bool IsInitialized() const
        {
            return mInitialized;
        }
        inline const VulkanTexture *GetBackbuffer() const
        {
            return mpBackbuffer;
        }
        VulkanBuffer *CreateBuffer(const BufferArgs &rArgs);
        VulkanMaterial *CreateMaterial(const VulkanMaterial::MaterialArgs &rArgs);
        VulkanMaterialDefinition *CreateMaterialDefinition(const VulkanMaterialDefinition::MaterialDefinitionArgs &rArgs);
        VulkanMesh *CreateMesh(const MeshArgs &rArgs);
        VulkanGraphicsContext *CreateGraphicsContext(const GraphicsContextArgs &rArgs);
        VulkanShader *CreateShader(const ShaderArgs &rArgs);
        VulkanTexture *CreateTexture(const TextureArgs &rArgs);
        void DestroyBuffer(const VulkanBuffer *pBuffer);
        void DestroyMaterial(const VulkanMaterial *pMaterial);
        void DestroyMaterialDefinition(const VulkanMaterialDefinition *pMaterialDefinition);
        void DestroyMesh(const VulkanMesh *pMesh);
        void DestroyGraphicsContext(const VulkanGraphicsContext *pGraphicsContext);
        void DestroyShader(const VulkanShader *pShader);
        void DestroyTexture(const VulkanTexture *pTexture);
        inline const VulkanMaterialDefinition *FindMaterialDefinition(const std::string &rName) const
        {
            auto it = std::find_if(mMaterialDefinitions.cbegin(), mMaterialDefinitions.cend(), [&rName](const auto &pMaterialDefinition)
                                   { return strcmp(pMaterialDefinition->GetName().c_str(), rName.c_str()) == 0; });
            if (it == mMaterialDefinitions.cend())
            {
                return nullptr;
            }
            return *it;
        }
        inline const VulkanShader *FindShader(const std::string &rName) const
        {
            auto it = std::find_if(mShaders.cbegin(), mShaders.cend(), [&rName](const auto &pShader)
                                   { return strcmp(pShader->GetName().c_str(), rName.c_str()) == 0; });
            if (it == mShaders.cend())
            {
                return nullptr;
            }
            return *it;
        }
        inline size_t GetTextureCount() const override
        {
            return mTextures.size();
        }
        inline const VulkanTexture *GetTextureAt(size_t i) const override
        {
            assert(i < mTextures.size());
            return mTextures[i];
        }
        inline size_t GetMaterialCount() const override
        {
            return mMaterials.size();
        }
        inline const VulkanMaterial *GetMaterialAt(size_t i) const override
        {
            assert(i < mMaterials.size());
            return mMaterials[i];
        }

    protected:
        VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~VulkanGraphicsSystem();

    private:
        std::vector<VulkanBuffer *> mBuffers;
        std::vector<VulkanMaterial *> mMaterials;
        std::vector<VulkanMaterialDefinition *> mMaterialDefinitions;
        std::vector<VulkanMesh *> mMeshes;
        std::vector<VulkanGraphicsContext *> mGraphicsContexts;
        std::vector<VulkanShader *> mShaders;
        std::vector<VulkanTexture *> mTextures;
        bool mInitialized{false};
        const VulkanTexture *mpBackbuffer;

        void Initialize() override;
        void Resize() {}
        void Present();
        void Finalize();
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;
    };

}

#endif

#endif