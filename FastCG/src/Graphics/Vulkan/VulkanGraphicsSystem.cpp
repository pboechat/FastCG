#ifdef FASTCG_VULKAN

#if defined FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/X11Application.h>
#endif
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Exception.h>
#include <FastCG/FastCG.h>
#include <FastCG/AssetSystem.h>

#include <cassert>

namespace FastCG
{
    VulkanGraphicsSystem::VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs) : BaseGraphicsSystem(rArgs)
    {
    }

    VulkanGraphicsSystem::~VulkanGraphicsSystem() = default;

    void VulkanGraphicsSystem::Initialize()
    {
        BaseGraphicsSystem::Initialize();

        // TODO

#ifdef FASTCG_LINUX
        X11Application::GetInstance()->CreateSimpleWindow();
#endif

        // Create backbuffer handler (fake texture)
        mpBackbuffer = CreateTexture({"Backbuffer",
                                      1,
                                      1,
                                      TextureType::TEXTURE_2D,
                                      TextureFormat::RGBA,
                                      {32, 32, 32, 32},
                                      TextureDataType::FLOAT,
                                      TextureFilter::POINT_FILTER,
                                      TextureWrapMode::CLAMP,
                                      false});

        mInitialized = true;
    }

#define DESTROY_ALL(containerName)             \
    for (const auto *pElement : containerName) \
    {                                          \
        delete pElement;                       \
    }                                          \
    containerName.clear()

    void VulkanGraphicsSystem::Finalize()
    {
        mInitialized = false;

        DESTROY_ALL(mMaterials);
        DESTROY_ALL(mMeshes);
        DESTROY_ALL(mShaders);
        DESTROY_ALL(mBuffers);
        DESTROY_ALL(mTextures);
        DESTROY_ALL(mGraphicsContexts);
    }

#define DECLARE_CREATE_METHOD(className, containerMember)        \
    Vulkan##className *VulkanGraphicsSystem::Create##className() \
    {                                                            \
        containerMember.emplace_back(new Vulkan##className{});   \
        return containerMember.back();                           \
    }

#define DECLARE_CREATE_METHOD_WITH_ARGS(className, containerMember, argType, argName) \
    Vulkan##className *VulkanGraphicsSystem::Create##className(argType argName)       \
    {                                                                                 \
        containerMember.emplace_back(new Vulkan##className{argName});                 \
        return containerMember.back();                                                \
    }

    DECLARE_CREATE_METHOD_WITH_ARGS(Buffer, mBuffers, const BufferArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Mesh, mMeshes, const MeshArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(MaterialDefinition, mMaterialDefinitions, const VulkanMaterialDefinition::MaterialDefinitionArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(GraphicsContext, mGraphicsContexts, const GraphicsContextArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Shader, mShaders, const ShaderArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Texture, mTextures, const TextureArgs &, rArgs)

    VulkanMaterial *VulkanGraphicsSystem::CreateMaterial(const VulkanMaterial::MaterialArgs &rArgs)
    {
        VulkanBuffer *pMaterialConstantBuffer = nullptr;
        const auto &rConstantBuffer = rArgs.pMaterialDefinition->GetConstantBuffer();
        if (rConstantBuffer.GetSize() > 0)
        {
            pMaterialConstantBuffer = CreateBuffer({rArgs.name + " Material Constant Buffer",
                                                    BufferType::UNIFORM,
                                                    BufferUsage::DYNAMIC,
                                                    rConstantBuffer.GetSize(),
                                                    rConstantBuffer.GetData()});
        }

        mMaterials.emplace_back(new VulkanMaterial(rArgs, pMaterialConstantBuffer));
        return mMaterials.back();
    }

#define DECLARE_DESTROY_METHOD(className, containerMember)                                                              \
    void VulkanGraphicsSystem::Destroy##className(const Vulkan##className *p##className)                                \
    {                                                                                                                   \
        assert(p##className != nullptr);                                                                                \
        auto it = std::find(containerMember.cbegin(), containerMember.cend(), p##className);                            \
        if (it != containerMember.cend())                                                                               \
        {                                                                                                               \
            containerMember.erase(it);                                                                                  \
            delete p##className;                                                                                        \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy " #className " '%s'", p##className->GetName().c_str()); \
        }                                                                                                               \
    }

    DECLARE_DESTROY_METHOD(Buffer, mBuffers)
    DECLARE_DESTROY_METHOD(Mesh, mMeshes)
    DECLARE_DESTROY_METHOD(MaterialDefinition, mMaterialDefinitions)
    DECLARE_DESTROY_METHOD(GraphicsContext, mGraphicsContexts)
    DECLARE_DESTROY_METHOD(Shader, mShaders)

    void VulkanGraphicsSystem::DestroyMaterial(const VulkanMaterial *pMaterial)
    {
        assert(pMaterial != nullptr);
        const auto *pMaterialConstantBuffer = pMaterial->GetConstantBuffer();
        if (pMaterialConstantBuffer != nullptr)
        {
            DestroyBuffer(pMaterialConstantBuffer);
        }

        auto it = std::find(mMaterials.cbegin(), mMaterials.cend(), pMaterial);
        if (it != mMaterials.cend())
        {
            mMaterials.erase(it);
            delete pMaterial;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy material '%s'", pMaterial->GetName().c_str());
        }
    }

    void VulkanGraphicsSystem::DestroyTexture(const VulkanTexture *pTexture)
    {
        assert(pTexture != nullptr);
        {
            auto it = std::find(mTextures.cbegin(), mTextures.cend(), pTexture);
            if (it != mTextures.cend())
            {
                mTextures.erase(it);
                delete pTexture;
            }
            else
            {
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy texture '%s'", pTexture->GetName().c_str());
            }
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