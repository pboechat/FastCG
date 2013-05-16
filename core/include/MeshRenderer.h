#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include <Renderer.h>
#include <Mesh.h>

#include <vector>
#include <algorithm>

COMPONENT(MeshRenderer, Renderer)
public:
	inline void AddMesh(Mesh* pMesh)
	{
		mMeshes.push_back(pMesh);
	}

	void RemoveMesh(Mesh* pMesh);

	inline std::vector<Mesh*> GetMeshes() const
	{
		return mMeshes;
	}

	inline virtual unsigned int GetNumberOfDrawCalls() const
	{
		return mMeshes.size();
	}

	virtual unsigned int GetNumberOfTriangles() const;

protected:
	virtual void OnRender();

private:
	std::vector<Mesh*> mMeshes;

};

#endif