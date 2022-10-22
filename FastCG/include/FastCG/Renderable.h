#ifndef FASTCG_RENDERABLE_H
#define FASTCG_RENDERABLE_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/Component.h>

namespace FastCG
{
	class Renderable : public Component
	{
		DECLARE_COMPONENT(Renderable, Component);

	public:
		inline Material *GetMaterial()
		{
			return mpMaterial;
		}

		inline const Material *GetMaterial() const
		{
			return mpMaterial;
		}

		inline Mesh *GetMesh()
		{
			return mpMesh;
		}

		inline const Mesh *GetMesh() const
		{
			return mpMesh;
		}

	private:
		Renderable(GameObject *pGameObject, Material *pMaterial, Mesh *pMesh) : Component(pGameObject), mpMaterial(pMaterial), mpMesh(pMesh) {}

		Material *const mpMaterial{nullptr};
		Mesh *const mpMesh{nullptr};
	};

}

#endif