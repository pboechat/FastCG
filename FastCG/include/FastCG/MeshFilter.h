#ifndef FASTCG_MESH_FILTER_H_
#define FASTCG_MESH_FILTER_H_

#include <FastCG/Component.h>
#include <FastCG/Material.h>

#include <memory>

namespace FastCG
{
	class MeshFilter : public Component
	{
		DECLARE_COMPONENT(MeshFilter, Component);

	public:
		inline std::shared_ptr<Material> GetMaterial()
		{
			return mpMaterial;
		}

		void SetMaterial(const std::shared_ptr<Material>& pMaterial);

		void OnInstantiate() override
		{
			mpMaterial = 0;
		}

	private:
		std::shared_ptr<Material> mpMaterial;

	};

}

#endif