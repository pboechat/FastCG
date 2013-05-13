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

protected:
	virtual void OnRender() = 0;

};

#endif