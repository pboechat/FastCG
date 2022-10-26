#ifndef FASTCG_BASE_RENDERING_SYSTEM_H
#define FASTCG_BASE_RENDERING_SYSTEM_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderingPath.h>
#include <FastCG/BaseTexture.h>
#include <FastCG/BaseShader.h>
#include <FastCG/BaseMesh.h>
#include <FastCG/BaseMaterial.h>
#include <FastCG/BaseBuffer.h>

#include <glm/glm.hpp>
#include <imgui.h>

#ifdef FASTCG_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

namespace FastCG
{
    struct RenderBatch;
    class PointLight;
    class DirectionalLight;
    class Camera;
    class BaseApplication;

    struct RenderingSystemArgs
    {
        RenderingPath renderingPath;
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        const glm::vec4 &rClearColor;
        const glm::vec4 &rAmbientLight;
        const std::vector<DirectionalLight *> &rDirectionalLights;
        const std::vector<PointLight *> &rPointLights;
        const std::vector<const RenderBatch *> &rRenderBatches;
        RenderingStatistics &rRenderingStatistics;
    };

    template <class BufferT, class MaterialT, class MeshT, class ShaderT, class TextureT>
    class BaseRenderingSystem
    {
    public:
        using Buffer = BufferT;
        using Material = MaterialT;
        using Mesh = MeshT;
        using Shader = ShaderT;
        using Texture = TextureT;

        // Template interface
        void Render(const Camera *pMainCamera);
        void RenderImGui(const ImDrawData *pImDrawData);
        void Present();
        void Initialize();
        void PostInitialize();
        void Resize();
        void Finalize();
#ifdef FASTCG_LINUX
        XVisualInfo *GetVisualInfo();
#endif
        Buffer *CreateBuffer(const BufferArgs &rArgs);
        Material *CreateMaterial(const MeshArgs &rArgs);
        Mesh *CreateMesh(const MeshArgs &rArgs);
        Shader *CreateShader(const ShaderArgs &rArgs);
        Texture *CreateTexture(const TextureArgs &rArgs);
        void DestroyBuffer(const Buffer *pBuffer);
        void DestroyMaterial(const Material *pMaterial);
        void DestroyMesh(const Mesh *pMesh);
        void DestroyShader(const Shader *pShader);
        void DestroyTexture(const Texture *pTexture);
        const Shader *FindShader(const std::string &rName) const;

        friend class BaseApplication;

    protected:
        const RenderingSystemArgs mArgs;

        BaseRenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs) {}
    };

}

#endif