#ifndef UNORDEREDSINGLEBATCHINGSTRATEGY_H_
#define UNORDEREDSINGLEBATCHINGSTRATEGY_H_

#include <RenderBatchingStrategy.h>

class UnorderedSingleGroupBatchingStrategy : public RenderBatchingStrategy
{
public:
	UnorderedSingleGroupBatchingStrategy(std::vector<RenderBatch*>& rRenderBatches) :
		RenderBatchingStrategy(rRenderBatches)
	{
	}

	virtual void AddMeshFilter(MeshFilter* pMeshFilter);
	virtual void RemoveMeshFilter(MeshFilter* pMeshFilter);

};

#endif