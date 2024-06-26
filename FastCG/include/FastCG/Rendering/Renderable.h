#ifndef FASTCG_RENDERABLE_H
#define FASTCG_RENDERABLE_H

#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Rendering/RenderingSystem.h>
#include <FastCG/World/Component.h>

#include <memory>

namespace FastCG
{
    class Renderable final : public Component
    {
        FASTCG_DECLARE_COMPONENT(Renderable, Component);

    public:
        inline const std::shared_ptr<Material> &GetMaterial() const
        {
            return mpMaterial;
        }

        inline void SetMaterial(const std::shared_ptr<Material> &pMaterial)
        {
            RenderingSystem::GetInstance()->UnregisterRenderable(this);
            mpMaterial = pMaterial;
            RenderingSystem::GetInstance()->RegisterRenderable(this);
        }

        inline const std::shared_ptr<Mesh> &GetMesh() const
        {
            return mpMesh;
        }

        inline void SetMesh(const std::shared_ptr<Mesh> &pMesh)
        {
            RenderingSystem::GetInstance()->UnregisterRenderable(this);
            mpMesh = pMesh;
            RenderingSystem::GetInstance()->RegisterRenderable(this);
        }

        inline bool IsShadowCaster() const
        {
            return mShadowCaster;
        }

        inline void SetShadowCaster(bool shadowCaster)
        {
            RenderingSystem::GetInstance()->UnregisterRenderable(this);
            mShadowCaster = shadowCaster;
            RenderingSystem::GetInstance()->RegisterRenderable(this);
        }

        inline bool IsSkybox() const
        {
            return mSkybox;
        }

        inline void SetSkybox(bool skybox)
        {
            RenderingSystem::GetInstance()->UnregisterRenderable(this);
            mSkybox = skybox;
            RenderingSystem::GetInstance()->RegisterRenderable(this);
        }

    protected:
        void OnInstantiate() override
        {
            RenderingSystem::GetInstance()->RegisterRenderable(this);
        }

        void OnRegisterInspectableProperties() override
        {
            RegisterInspectableProperty(this, "Material", &Renderable::GetMaterial, &Renderable::SetMaterial);
            RegisterInspectableProperty(this, "Mesh", &Renderable::GetMesh, &Renderable::SetMesh);
            RegisterInspectableProperty(this, "Shadow Caster", &Renderable::IsShadowCaster,
                                        &Renderable::SetShadowCaster);
            RegisterInspectableProperty(this, "Skybox", &Renderable::IsSkybox, &Renderable::SetSkybox);
        }

        void OnDestroy() override
        {
            RenderingSystem::GetInstance()->UnregisterRenderable(this);
        }

    private:
        std::shared_ptr<Material> mpMaterial{nullptr};
        std::shared_ptr<Mesh> mpMesh{nullptr};
        bool mShadowCaster{false};
        bool mSkybox{false};

        Renderable(GameObject *pGameObject, std::unique_ptr<Material> &&pMaterial, std::unique_ptr<Mesh> &&pMesh,
                   bool isShadowCaster = false, bool isSkybox = false)
            : Component(pGameObject), mpMaterial(std::move(pMaterial)), mpMesh(std::move(pMesh)),
              mShadowCaster(isShadowCaster), mSkybox(isSkybox)
        {
        }

        Renderable(GameObject *pGameObject, const std::shared_ptr<Material> &pMaterial,
                   const std::shared_ptr<Mesh> &pMesh, bool isShadowCaster = false, bool isSkybox = false)
            : Component(pGameObject), mpMaterial(pMaterial), mpMesh(pMesh), mShadowCaster(isShadowCaster),
              mSkybox(isSkybox)
        {
        }
    };

}

#endif