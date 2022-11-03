#ifndef FASTCG_BASE_RENDERING_SYSTEM_H
#define FASTCG_BASE_RENDERING_SYSTEM_H

#include <FastCG/BaseTexture.h>
#include <FastCG/BaseShader.h>
#include <FastCG/BaseMesh.h>
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
    };

    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    class BaseRenderingSystem
    {
    public:
        using Buffer = BufferT;
        using Material = MaterialT;
        using Mesh = MeshT;
        using RenderingContext = RenderingContextT;
        using Shader = ShaderT;
        using Texture = TextureT;

        static_assert(std::is_same_v<Material::Buffer, Buffer>, "Material::Buffer type must be the same as Buffer type");
        static_assert(std::is_same_v<Material::Shader, Shader>, "Material::Shader type must be the same as Shader type");
        static_assert(std::is_same_v<Material::Texture, Texture>, "Material::Texture type must be the same as Texture type");
        static_assert(std::is_same_v<Mesh::Buffer, Buffer>, "Mesh::Buffer type must be the same as Buffer type");
        static_assert(std::is_same_v<RenderingContext::Buffer, Buffer>, "RenderingContext::Buffer type must be the same as Buffer type");
        static_assert(std::is_same_v<RenderingContext::Shader, Shader>, "RenderingContext::Shader type must be the same as Shader type");
        static_assert(std::is_same_v<RenderingContext::Texture, Texture>, "RenderingContext::Texture type must be the same as Texture type");

        // Template interface
        const Texture *GetBackbuffer() const;
        uint32_t GetScreenWidth() const;
        uint32_t GetScreenHeight() const;
        Buffer *CreateBuffer(const BufferArgs &rArgs);
        Material *CreateMaterial(const MeshArgs &rArgs);
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
        const Shader *FindShader(const std::string &rName) const;
#ifdef FASTCG_LINUX
        XVisualInfo *GetVisualInfo();
#endif

    protected:
        const RenderingSystemArgs mArgs;

        BaseRenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs) {}

        // Template interface
        void Initialize();
        void Resize();
        void Present();
        void Finalize();

        friend class BaseApplication;
    };

}

#endif