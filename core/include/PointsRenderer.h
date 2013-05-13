#ifndef POINTSRENDERER_H_
#define POINTSRENDERER_H_

#include <Renderer.h>
#include <Points.h>

COMPONENT(PointsRenderer, Renderer)
public:
	inline void SetPoints(Points* pPoints)
	{
		mpPoints = pPoints;
	}

	inline Points* GetPoints() const
	{
		return mpPoints;
	}

	virtual void OnInstantiate()
	{
		mpPoints = 0;
	}

protected:
	virtual void OnRender();

private:
	Points* mpPoints;

};

#endif