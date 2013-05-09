#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <Pointer.h>

class Drawable
{
public:
	Drawable() :
	  mEnabled(true)
	{
	}

	void Draw()
	{
		if (mEnabled)
		{
			OnDraw();
		}
	}

	inline bool IsEnabled() const
	{
		return mEnabled;
	}

	inline void SetEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

protected:
	virtual void OnDraw() = 0;

private:
	bool mEnabled;

};

typedef Pointer<Drawable> DrawablePtr;

#endif