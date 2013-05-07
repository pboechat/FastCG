#ifndef BSPLINESAPPLICATION_H_
#define BSPLINESAPPLICATION_H_

#include <Application.h>
#include <LineStrip.h>
#include <BSpline.h>

#include <glm/glm.hpp>

#include <vector>

class BSplinesApplication : public Application
{
public:
	BSplinesApplication();
	virtual ~BSplinesApplication();

	virtual void OnStart();
	virtual void BeforeDisplay();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnKeyPress(int key);

private:
	static const unsigned int MIN_DEGREE;
	static const unsigned int MAX_DEGREE;
	static const unsigned int NUM_BSPLINE_SAMPLES;
	static const glm::vec4 BSPLINE_COLOR;
	static const glm::vec4 CONTROL_POLYGON_COLOR;

	enum State
	{
		BSPLINE_DEFINITION,
		BSPLINE_MANIPULATION

	};

	unsigned int mBSplineDegree;
	std::vector<glm::vec2> mBSplineControlPoints;
	LineStripPtr mControlPolygonPtr;
	LineStripPtr mBSplineCurvePtr;
	BSplinePtr mBSplinePtr;
	State mState;

	void SetState(State state);
	void AddControlPoint(const glm::vec3& rPoint);
	void RemoveControlPoint();
	void UpdateBSplineControlPointsLineStrip();

};

#endif