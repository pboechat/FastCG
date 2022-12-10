#ifndef FASTCG_RENDERABLE_H
#define FASTCG_RENDERABLE_H

#include <FastCG/RenderingSystem.h>
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
		void OnRegisterInspectableProperties() override
		{
			RegisterInspectableProperty(this, "Material", &Renderable::GetMaterialName);
			RegisterInspectableProperty(this, "Mesh", &Renderable::GetMeshName);
			RegisterInspectableProperty(this, "Shadow Caster", &Renderable::IsShadowCaster);
		}

	private:
		const Material *const mpMaterial{nullptr};
		const Mesh *const mpMesh{nullptr};
		const bool mIsShadowCaster{false};

		Renderable(GameObject *pGameObject, const Material *pMaterial, const Mesh *pMesh, bool isShadowCaster = false) : Component(pGameObject), mpMaterial(pMaterial), mpMesh(pMesh), mIsShadowCaster(isShadowCaster) {}

		inline const std::string &GetMaterialName() const
		{
			static const std::string sUnassigned = "<unassigned>";
			return mpMaterial != nullptr ? mpMaterial->GetName() : sUnassigned;
		}

		inline const std::string &GetMeshName() const
		{
			static const std::string sUnassigned = "<unassigned>";
			return mpMesh != nullptr ? mpMesh->GetName() : sUnassigned;
		}
	};

}

#endif