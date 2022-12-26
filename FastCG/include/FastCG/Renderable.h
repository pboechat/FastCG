#ifndef FASTCG_RENDERABLE_H
#define FASTCG_RENDERABLE_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/GraphicsSystem.h>
#include <FastCG/Component.h>

namespace FastCG
{
	class Renderable : public Component
	{
		FASTCG_DECLARE_COMPONENT(Renderable, Component);

	public:
		inline const Material *GetMaterial() const
		{
			return mpMaterial;
		}

		inline void SetMaterial(const Material *pMaterial)
		{
			RenderingSystem::GetInstance()->UnregisterRenderable(this);
			mpMaterial = pMaterial;
			RenderingSystem::GetInstance()->RegisterRenderable(this);
		}

		inline const Mesh *GetMesh() const
		{
			return mpMesh;
		}

		inline void SetMesh(const Mesh *pMesh)
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

	protected:
		void OnInstantiate() override
		{
			RenderingSystem::GetInstance()->RegisterRenderable(this);
		}

		void OnRegisterInspectableProperties() override
		{
			RegisterInspectableProperty(this, "Material", &Renderable::GetMaterial, &Renderable::SetMaterial);
			RegisterInspectableProperty(this, "Mesh", &Renderable::GetMesh, &Renderable::SetMesh);
			RegisterInspectableProperty(this, "Shadow Caster", &Renderable::IsShadowCaster, &Renderable::SetShadowCaster);
		}

		void OnDestroy() override
		{
			RenderingSystem::GetInstance()->UnregisterRenderable(this);
		}

	private:
		const Material *mpMaterial{nullptr};
		const Mesh *mpMesh{nullptr};
		bool mShadowCaster{false};

		Renderable(GameObject *pGameObject, const Material *pMaterial, const Mesh *pMesh, bool isShadowCaster = false) : Component(pGameObject),
																														 mpMaterial(pMaterial),
																														 mpMesh(pMesh),
																														 mShadowCaster(isShadowCaster)
		{
		}
	};

}

#endif