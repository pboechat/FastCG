#ifndef RENDERER_H_
#define RENDERER_H_

#include <Component.h>

class Renderer : public Component
{
	DECLARE_TYPE;

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

typedef Pointer<Renderer> RendererPtr;

#endif