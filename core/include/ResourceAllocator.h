#ifndef RESOURCEALLOCATOR_H_
#define RESOURCEALLOCATOR_H_

class ResourceAllocator
{
public:
	virtual bool HasAllocatedResources() = 0;
	virtual void AllocateResources() = 0;
	virtual void ReallocateResources() = 0;

};

#endif