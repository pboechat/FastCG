#ifndef BSPLINESEDITOR_H_
#define BSPLINESEDITOR_H_

#include <Behaviour.h>
#include <BSplineCurve.h>

#include <glm/glm.hpp>

class BSplinesEditor : public Behaviour
{
	DECLARE_TYPE;

public:
	BSplinesEditor();

	virtual void OnUpdate(float time, float deltaTime);

	inline void SetBSplineGameObject(const GameObjectPtr& bSplineGameObjectPtr)
	{
		mBSplineGameObjectPtr = bSplineGameObjectPtr;
	}

	inline void SetControlPolygonGameObject(const GameObjectPtr& controlPolygonGameObjectPtr)
	{
		mControlPolygonGameObjectPtr = controlPolygonGameObjectPtr;
	}

	inline void SetControlPointsGameObject(const GameObjectPtr& controlPointsGameObjectPtr)
	{
		mControlPointsGameObjectPtr = controlPointsGameObjectPtr;
	}

	inline void SetKnotsGameObject(const GameObjectPtr& knotsGameObjectPtr)
	{
		mKnotsGameObjectPtr = knotsGameObjectPtr;
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
	GameObjectPtr mControlPolygonGameObjectPtr;
	GameObjectPtr mBSplineGameObjectPtr;
	GameObjectPtr mControlPointsGameObjectPtr;
	GameObjectPtr mKnotsGameObjectPtr;
	BSplinePtr mBSplinePtr;
	State mState;
	int mSelectedControlPointIndex;
	float mLastKeyPressTime;
	float mLastRightMouseButtonClickTime;
	float mLastLeftMouseButtonClickTime;

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