#ifndef FASTCG_BASE_RENDERING_SYSTEM_H
#define FASTCG_BASE_RENDERING_SYSTEM_H

#include <FastCG/BaseTexture.h>
#include <FastCG/BaseShader.h>
#include <FastCG/BaseMesh.h>
#include <FastCG/BaseMaterialDefinition.h>
#include <FastCG/BaseMaterial.h>
#include <FastCG/BaseBuffer.h>

#include <glm/glm.hpp>

#ifdef FASTCG_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <type_traits>
#include <string>

namespace FastCG
{
    class BaseApplication;

    struct RenderingSystemArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        bool vsync;
    };

    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    class BaseRenderingSystem
    {
    public:
        using Buffer = BufferT;
        using Material = MaterialT;
        using MaterialDefinition = typename MaterialT::MaterialDefinition;
        using Mesh = MeshT;
        using RenderingContext = RenderingContextT;
        using Shader = ShaderT;
        using Texture = TextureT;

        static_assert(std::is_same<typename Material::Buffer, Buffer>::value, "Material::Buffer type must be the same as Buffer type");
        static_assert(std::is_same<typename Material::Shader, Shader>::value, "Material::Shader type must be the same as Shader type");
        static_assert(std::is_same<typename Material::Texture, Texture>::value, "Material::Texture type must be the same as Texture type");
        static_assert(std::is_same<typename Mesh::Buffer, Buffer>::value, "Mesh::Buffer type must be the same as Buffer type");
        static_assert(std::is_same<typename RenderingContext::Buffer, Buffer>::value, "RenderingContext::Buffer type must be the same as Buffer type");
        static_assert(std::is_same<typename RenderingContext::Shader, Shader>::value, "RenderingContext::Shader type must be the same as Shader type");
        static_assert(std::is_same<typename RenderingContext::Texture, Texture>::value, "RenderingContext::Texture type must be the same as Texture type");

        // Template interface methods
        bool IsInitialized() const;
        const Texture *GetBackbuffer() const;
        inline uint32_t GetScreenWidth() const
        {
            return mArgs.rScreenWidth;
        }
        inline uint32_t GetScreenHeight() const
        {
            return mArgs.rScreenHeight;
        }
        Buffer *CreateBuffer(const BufferArgs &rArgs);
        MaterialDefinition *CreateMaterialDefinition(const typename MaterialDefinition::MaterialDefinitionArgs &rArgs);
        Material *CreateMaterial(const typename Material::MaterialArgs &rArgs);
        Mesh *CreateMesh(const MeshArgs &rArgs);
        RenderingContext *CreateRenderingContext();
        Shader *CreateShader(const ShaderArgs &rArgs);
        Texture *CreateTexture(const TextureArgs &rArgs);
        void DestroyBuffer(const Buffer *pBuffer);
        void DestroyMaterial(const Material *pMaterial);
        void DestroyMesh(const Mesh *pMesh);
        void DestroyRenderingContext(const RenderingContext *pRenderingContext);
        void DestroyShader(const Shader *pShader);
        void DestroyTexture(const Texture *pTexture);
        const MaterialDefinition *FindMaterialDefinition(const std::string &rName) const;
        const Shader *FindShader(const std::string &rName) const;
#if defined FASTCG_WINDOWS
        void SetupPixelFormat() const;
#elif defined FASTCG_LINUX
        XVisualInfo *GetVisualInfo();
#endif
        virtual size_t GetTextureCount() const = 0;
        virtual const Texture *GetTextureAt(size_t i) const = 0;
        virtual size_t GetMaterialCount() const = 0;
        virtual const Material *GetMaterialAt(size_t i) const = 0;

    protected:
        const RenderingSystemArgs mArgs;
#ifdef _DEBUG
        const Texture *mpSelectedTexture{nullptr};
        const Material *mpSelectedMaterial{nullptr};
        bool mShowTextureBrowser{false};
        bool mShowMaterialBrowser{false};
#endif

        BaseRenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs)
        {
        }
        virtual ~BaseRenderingSystem() = default;

        // Non-interface methods
        virtual void Initialize();
#ifdef _DEBUG
        inline void DebugMenuCallback(int result);
        inline void DebugMenuItemCallback(int &result);
#endif

        // Template interface methods
        void Resize();
        void Present();
        void Finalize();
        double GetLastPresentElapsedTime();

        friend class BaseApplication;
    };

}

#include <FastCG/BaseRenderingSystem.inc>

#endif