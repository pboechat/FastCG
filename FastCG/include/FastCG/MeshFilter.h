#ifndef FASTCG_MESH_FILTER_H
#define FASTCG_MESH_FILTER_H

#include <FastCG/Component.h>
#include <FastCG/Material.h>

#include <memory>

namespace FastCG
{
	class MeshFilter : public Component
	{
		DECLARE_COMPONENT(MeshFilter, Component);

	public:
		inline const std::shared_ptr<Material> &GetMaterial() const
		{
			return mpMaterial;
		}

		void SetMaterial(const std::shared_ptr<Material> &pMaterial);

	private:
		std::shared_ptr<Material> mpMaterial{nullptr};
	};

}

#endif