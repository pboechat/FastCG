#ifndef FASTCG_MESH_RENDERER_H_
#define FASTCG_MESH_RENDERER_H_

#include <FastCG/Renderer.h>
#include <FastCG/Mesh.h>

#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>

namespace FastCG
{
	class MeshRenderer : public Renderer
	{
		DECLARE_COMPONENT(MeshRenderer, Renderer);

	public:
		inline void AddMesh(const std::shared_ptr<Mesh> &pMesh)
		{
			mMeshes.emplace_back(pMesh);
		}

		inline void RemoveMesh(const std::shared_ptr<Mesh> &pMesh)
		{
			auto it = std::find(mMeshes.begin(), mMeshes.end(), pMesh);
			assert(it != mMeshes.end());
			mMeshes.erase(it);
		}

		inline const std::vector<std::shared_ptr<Mesh>> &GetMeshes() const
		{
			return mMeshes;
		}

		inline size_t GetNumberOfDrawCalls() const override
		{
			return mMeshes.size();
		}

		inline size_t GetNumberOfTriangles() const override
		{
			size_t totalNumberOfTriangles = 0;
			for (const auto &pMesh : mMeshes)
			{
				totalNumberOfTriangles += pMesh->GetNumberOfTriangles();
			}
			return totalNumberOfTriangles;
		}

	protected:
		void OnRender() override;

	private:
		std::vector<std::shared_ptr<Mesh>> mMeshes;
	};

}

#endif