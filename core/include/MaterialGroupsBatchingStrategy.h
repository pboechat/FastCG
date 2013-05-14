#ifndef MATERIALGROUPSBATCHINGSTRATEGY_H_
#define MATERIALGROUPSBATCHINGSTRATEGY_H_

#include <RenderBatchingStrategy.h>

class MaterialGroupsBatchingStrategy : public RenderBatchingStrategy
{
public:
	MaterialGroupsBatchingStrategy(std::vector<RenderBatch*>& rRenderBatches) :
		RenderBatchingStrategy(rRenderBatches)
	{
	}

	virtual void AddMeshFilter(MeshFilter* pMeshFilter);
	virtual void RemoveMeshFilter(MeshFilter* pMeshFilter);

};

#endif