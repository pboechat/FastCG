#ifndef FASTCG_BASE_RENDERING_SYSTEM_H
#define FASTCG_BASE_RENDERING_SYSTEM_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderingPath.h>
#include <FastCG/BaseTexture.h>
#include <FastCG/BaseShader.h>
#include <FastCG/BaseMesh.h>
#include <FastCG/BaseMaterial.h>

#include <glm/glm.hpp>
#include <imgui.h>

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

    template <class MaterialT, class MeshT, class ShaderT, class TextureT>
    class BaseRenderingSystem
    {
    public:
        using Material = MaterialT;
        using Mesh = MeshT;
        using Shader = ShaderT;
        using Texture = TextureT;

        // Template interface
        void Render(const Camera *pMainCamera);
        void RenderImGui(const ImDrawData *pImDrawData);
        void Initialize();
        void PostInitialize();
        void Resize();
        void Finalize();
        Material *CreateMaterial(const MeshArgs &rArgs);
        Mesh *CreateMesh(const MeshArgs &rArgs);
        Shader *CreateShader(const ShaderArgs &rArgs);
        Texture *CreateTexture(const TextureArgs &rArgs);
        const Shader *FindShader(const std::string &rName) const;

        friend class BaseApplication;

    protected:
        const RenderingSystemArgs mArgs;

        BaseRenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs) {}
    };

}

#endif