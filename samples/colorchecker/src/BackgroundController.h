#ifndef BACKGROUNDCONTROLLER_H_
#define BACKGROUNDCONTROLLER_H_

#include <Behaviour.h>
#include <sRGBColor.h>

#include <vector>

COMPONENT(BackgroundController, Behaviour)
public:
	void SetBaseColors(const std::vector<sRGBColor>& rBaseColors)
	{
		mBaseColors = rBaseColors;
	}

	void SetVerticalNumberOfColorPatches(unsigned int verticalNumberOfColorPatches)
	{
		mVerticalNumberOfColorPatches = verticalNumberOfColorPatches;
	}

	void SetHorizontalNumberOfColorPatches(unsigned int horizontalNumberOfColorPatches)
	{
		mHorizontalNumberOfColorPatches = horizontalNumberOfColorPatches;
	}

protected:
	virtual void OnUpdate(float time, float deltaTime);

private:
	float mLastLeftMouseButtonClickTime;
	std::vector<sRGBColor> mBaseColors;
	unsigned int mVerticalNumberOfColorPatches;
	unsigned int mHorizontalNumberOfColorPatches;

	int GetColorPatchIndex(int x, int y);

};

#endif