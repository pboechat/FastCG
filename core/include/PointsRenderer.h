#ifndef POINTSRENDERER_H_
#define POINTSRENDERER_H_

#include <Renderer.h>
#include <Points.h>

class PointsRenderer : public Renderer
{
	DECLARE_TYPE;

public:
	inline void SetPoints(const PointsPtr& pointsPtr)
	{
		mPointsPtr = pointsPtr;
	}

	inline PointsPtr GetPoints() const
	{
		return mPointsPtr;
	}

protected:
	virtual void OnRender();

private:
	PointsPtr mPointsPtr;

};

typedef Pointer<PointsRenderer> PointsRendererPtr;

#endif