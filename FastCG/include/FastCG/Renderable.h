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

		inline const Mesh *GetMesh() const
		{
			return mpMesh;
		}

		inline bool IsShadowCaster() const
		{
			return mIsShadowCaster;
		}

	protected:
		void OnInstantiate() override
		{
			RenderingSystem::GetInstance()->RegisterRenderable(this);
		}

		void OnRegisterInspectableProperties() override
		{
			RegisterInspectableProperty(this, "Material", &Renderable::GetMaterial);
			RegisterInspectableProperty(this, "Mesh", &Renderable::GetMesh);
			RegisterInspectableProperty(this, "Shadow Caster", &Renderable::IsShadowCaster);
		}

		void OnDestroy() override
		{
			RenderingSystem::GetInstance()->UnregisterRenderable(this);
		}

	private:
		const Material *const mpMaterial{nullptr};
		const Mesh *const mpMesh{nullptr};
		const bool mIsShadowCaster{false};

		Renderable(GameObject *pGameObject, const Material *pMaterial, const Mesh *pMesh, bool isShadowCaster = false) : Component(pGameObject),
																														 mpMaterial(pMaterial),
																														 mpMesh(pMesh),
																														 mIsShadowCaster(isShadowCaster)
		{
		}
	};

}

#endif