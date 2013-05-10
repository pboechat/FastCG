#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include <Renderer.h>
#include <Mesh.h>

class MeshRenderer : public Renderer
{
	DECLARE_TYPE;

public:
	inline void SetMesh(const MeshPtr& meshPtr)
	{
		mMeshPtr = meshPtr;
	}

	inline MeshPtr GetMesh() const
	{
		return mMeshPtr;
	}

protected:
	virtual void OnRender();

private:
	MeshPtr mMeshPtr;

};

typedef Pointer<MeshRenderer> MeshRendererPtr;

#endif