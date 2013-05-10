#ifndef LINERENDERER_H_
#define LINERENDERER_H_

#include <Renderer.h>
#include <LineStrip.h>

class GameObject;

class LineRenderer : public Renderer
{
	DECLARE_TYPE;

public:
	inline void SetLineStrip(const LineStripPtr& lineStripPtr)
	{
		mLineStripPtr = lineStripPtr;
	}

	inline LineStripPtr GetLineStrip() const
	{
		return mLineStripPtr;
	}

protected:
	virtual void OnRender();

private:
	LineStripPtr mLineStripPtr;

};

typedef Pointer<LineRenderer> LineRendererPtr;

#endif