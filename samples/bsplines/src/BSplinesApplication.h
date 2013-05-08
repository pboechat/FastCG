#ifndef BSPLINESAPPLICATION_H_
#define BSPLINESAPPLICATION_H_

#include <Application.h>
#include <LineStrip.h>
#include <Points.h>
#include <BSpline.h>

#include <glm/glm.hpp>

#include <vector>

class BSplinesApplication : public Application
{
public:
	BSplinesApplication();
	virtual ~BSplinesApplication();

	virtual void BeforeDisplay();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnKeyPress(int key);

private:
	static const unsigned int MIN_DEGREE;
	static const unsigned int NUM_BSPLINE_SAMPLES;
	static const glm::vec4 BSPLINE_COLOR;
	static const glm::vec4 CONTROL_POLYGON_COLOR;
	static const glm::vec4 CONTROL_POINT_COLOR;
	static const glm::vec4 SELECTED_CONTROL_POINT_COLOR;
	static const float CONTROL_POINT_SIDE;
	static const glm::vec4 CONTROL_POINT_SELECTION_AREA;

	enum State
	{
		BSPLINE_DEFINITION,
		BSPLINE_MANIPULATION

	};

	unsigned int mBSplineDegree;
	std::vector<glm::vec2> mBSplineControlPoints;
	LineStripPtr mBSplineControlPolygonPtr;
	LineStripPtr mBSplineCurvePtr;
	PointsPtr mBSplineControlPointsPtr;
	BSplinePtr mBSplinePtr;
	State mState;
	int mSelectedControlPointIndex;

	void SetState(State state);
	void RemoveControlPoint();
	void UpdateControlPoints();
	int GetControlPointIndex(const glm::vec3& rPoint);
};

#endif