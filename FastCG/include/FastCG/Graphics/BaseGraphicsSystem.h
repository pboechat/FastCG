#ifndef FASTCG_BASE_GRAPHICS_SYSTEM_H
#define FASTCG_BASE_GRAPHICS_SYSTEM_H

#include <FastCG/Graphics/BaseTexture.h>
#include <FastCG/Graphics/BaseShader.h>
#include <FastCG/Graphics/BaseMesh.h>
#include <FastCG/Graphics/BaseMaterialDefinition.h>
#include <FastCG/Graphics/BaseMaterial.h>
#include <FastCG/Graphics/BaseBuffer.h>

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

    struct GraphicsSystemArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        bool vsync;
    };

    template <class BufferT, class MaterialT, class MeshT, class GraphicsContextT, class ShaderT, class TextureT>
    class BaseGraphicsSystem
    {
    public:
        using Buffer = BufferT;
        using Material = MaterialT;
        using MaterialDefinition = typename MaterialT::MaterialDefinition;
        using Mesh = MeshT;
        using GraphicsContext = GraphicsContextT;
        using Shader = ShaderT;
        using Texture = TextureT;

        static_assert(std::is_same<typename Material::Buffer, Buffer>::value, "Material::Buffer type must be the same as Buffer type");
        static_assert(std::is_same<typename Material::Shader, Shader>::value, "Material::Shader type must be the same as Shader type");
        static_assert(std::is_same<typename Material::Texture, Texture>::value, "Material::Texture type must be the same as Texture type");
        static_assert(std::is_same<typename Mesh::Buffer, Buffer>::value, "Mesh::Buffer type must be the same as Buffer type");
        static_assert(std::is_same<typename GraphicsContext::Shader, Shader>::value, "GraphicsContext::Shader type must be the same as Shader type");
        static_assert(std::is_same<typename GraphicsContext::Texture, Texture>::value, "GraphicsContext::Texture type must be the same as Texture type");
        static_assert(std::is_same<typename GraphicsContext::Buffer, Buffer>::value, "GraphicsContext::Buffer type must be the same as Buffer type");

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
        GraphicsContext *CreateGraphicsContext(const GraphicsContextArgs &rArgs);
        Shader *CreateShader(const ShaderArgs &rArgs);
        Texture *CreateTexture(const TextureArgs &rArgs);
        void DestroyBuffer(const Buffer *pBuffer);
        void DestroyMaterial(const Material *pMaterial);
        void DestroyMesh(const Mesh *pMesh);
        void DestroyGraphicsContext(const GraphicsContext *pGraphicsContext);
        void DestroyShader(const Shader *pShader);
        void DestroyTexture(const Texture *pTexture);
        const MaterialDefinition *FindMaterialDefinition(const std::string &rName) const;
        const Shader *FindShader(const std::string &rName) const;

#if defined FASTCG_WINDOWS
        void SetupPixelFormat() const;
#elif defined FASTCG_LINUX
        virtual XVisualInfo *GetVisualInfo();
#endif
        virtual size_t GetTextureCount() const = 0;
        virtual const Texture *GetTextureAt(size_t i) const = 0;
        virtual size_t GetMaterialCount() const = 0;
        virtual const Material *GetMaterialAt(size_t i) const = 0;
#ifdef _DEBUG
        inline const Texture *GetSelectedTexture() const
        {
            return mpSelectedTexture;
        }

        inline void SetSelectedTexture(const Texture *pTexture)
        {
            mShowTextureBrowser = true;
            mpSelectedTexture = pTexture;
        }

        inline const Material *GetSelectedMaterial() const
        {
            return mpSelectedMaterial;
        }

        inline void SetSelectedMaterial(const Material *pMaterial)
        {
            mShowMaterialBrowser = true;
            mpSelectedMaterial = pMaterial;
        }

#endif

    protected:
        const GraphicsSystemArgs mArgs;
#ifdef _DEBUG
        const Texture *mpSelectedTexture{nullptr};
        const Material *mpSelectedMaterial{nullptr};
        bool mShowTextureBrowser{false};
        bool mShowMaterialBrowser{false};
#endif

        BaseGraphicsSystem(const GraphicsSystemArgs &rArgs) : mArgs(rArgs)
        {
        }
        virtual ~BaseGraphicsSystem() = default;

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
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;

        friend class BaseApplication;
    };

}

#include <FastCG/Graphics/BaseGraphicsSystem.inc>

#endif