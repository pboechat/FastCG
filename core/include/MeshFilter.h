#ifndef MESHFILTER_H_
#define MESHFILTER_H_

#include <Component.h>
#include <Material.h>

class MeshFilter : public Component
{
	DECLARE_TYPE;

public:
	inline MaterialPtr GetMaterial()
	{
		return mMaterialPtr;
	}

	inline void SetMaterial(const MaterialPtr& rMaterialPtr)
	{
		mMaterialPtr = rMaterialPtr;
	}

private:
	MaterialPtr mMaterialPtr;

};

typedef Pointer<MeshFilter> MeshFilterPtr;

#endif