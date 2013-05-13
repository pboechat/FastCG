#ifndef LINERENDERER_H_
#define LINERENDERER_H_

#include <Renderer.h>
#include <LineStrip.h>

class GameObject;

COMPONENT(LineRenderer, Renderer)
public:
	inline void SetLineStrip(LineStrip* pLineStrip)
	{
		mpLineStrip = pLineStrip;
	}

	inline const LineStrip* GetLineStrip()
	{
		return mpLineStrip;
	}

	virtual void OnInstantiate()
	{
		mpLineStrip = 0;
	}

protected:
	virtual void OnRender();

private:
	LineStrip* mpLineStrip;

};

#endif