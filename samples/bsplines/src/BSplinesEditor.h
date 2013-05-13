#ifndef BSPLINESEDITOR_H_
#define BSPLINESEDITOR_H_

#include <Behaviour.h>
#include <BSplineCurve.h>
#include <LineStrip.h>
#include <Points.h>

#include <glm/glm.hpp>

COMPONENT(BSplinesEditor, Behaviour)
public:
	virtual void OnInstantiate();
	virtual void OnDestroy();

	virtual void OnUpdate(float time, float deltaTime);

	inline void SetBSplineGameObject(GameObject* pBSplineGameObject)
	{
		mpBSplineGameObject = pBSplineGameObject;
	}

	inline void SetControlPolygonGameObject(GameObject* pControlPolygonGameObject)
	{
		mpControlPolygonGameObject = pControlPolygonGameObject;
	}

	inline void SetControlPointsGameObject(GameObject* pControlPointsGameObject)
	{
		mpControlPointsGameObject = pControlPointsGameObject;
	}

	inline void SetKnotsGameObject(GameObject* pKnotsGameObject)
	{
		mpKnotsGameObject = pKnotsGameObject;
	}

private:
	enum State
	{
		BSPLINE_DEFINITION,
		BSPLINE_MANIPULATION

	};

	static const unsigned int MIN_DEGREE;
	static const unsigned int NUM_BSPLINE_SAMPLES;
	static const glm::vec4 BSPLINE_COLOR;
	static const glm::vec4 KNOT_COLOR;
	static const glm::vec4 CONTROL_POLYGON_COLOR;
	static const glm::vec4 CONTROL_POINT_COLOR;
	static const glm::vec4 SELECTED_CONTROL_POINT_COLOR;
	static const float KNOT_SIZE;
	static const float CONTROL_POINT_SIZE;
	static const glm::vec4 CONTROL_POINT_SELECTION_AREA;

	unsigned int mBSplineDegree;
	std::vector<glm::vec2> mControlPoints;
	GameObject* mpControlPolygonGameObject;
	GameObject* mpBSplineGameObject;
	GameObject* mpControlPointsGameObject;
	GameObject* mpKnotsGameObject;
	BSplineCurve* mpBSplineCurve;
	State mState;
	int mSelectedControlPointIndex;
	float mLastKeyPressTime;
	float mLastRightMouseButtonClickTime;
	float mLastLeftMouseButtonClickTime;
	LineStrip* mpBSpline;
	Points* mpKnots;
	Points* mpControlPoints;
	LineStrip* mpControlPolygon;
	void SetState(State state);
	void RemoveLastControlPoint();
	void UpdateBSpline();
	void RemoveBSpline();
	void UpdateControlPoints();
	void RemoveControlPoints();
	int GetControlPointIndex(const glm::vec3& rPoint);
	void DrawGUI();

};

#endif