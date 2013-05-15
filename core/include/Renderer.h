#ifndef RENDERER_H_
#define RENDERER_H_

#include <Component.h>

ABSTRACT_COMPONENT(Renderer, Component)
public:
	inline void Render()
	{
		if (IsEnabled())
		{
			OnRender();
		}
	}

	inline virtual unsigned int GetNumberOfTriangles() const
	{
		return 0;
	}

protected:
	virtual void OnRender() = 0;

};

#endif