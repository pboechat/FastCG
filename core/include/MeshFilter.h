#ifndef MESHFILTER_H_
#define MESHFILTER_H_

#include <Component.h>
#include <Material.h>

COMPONENT(MeshFilter, Component)
public:
	inline Material* GetMaterial()
	{
		return mpMaterial;
	}

	void SetMaterial(Material* pMaterial);

	virtual void OnInstantiate()
	{
		mpMaterial = 0;
	}

private:
	Material* mpMaterial;

};

#endif