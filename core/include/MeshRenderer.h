#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include <Renderer.h>
#include <Mesh.h>

COMPONENT(MeshRenderer, Renderer)
public:
	inline void SetMesh(Mesh* pMesh)
	{
		mpMesh = pMesh;
	}

	inline Mesh* GetMesh() const
	{
		return mpMesh;
	}

	virtual void OnInstantiate()
	{
		mpMesh = 0;
	}

	inline virtual unsigned int GetNumberOfTriangles() const
	{
		return (mpMesh == 0) ? 0 : mpMesh->GetNumberOfTriangles();
	}

protected:
	virtual void OnRender();

private:
	Mesh* mpMesh;

};

#endif