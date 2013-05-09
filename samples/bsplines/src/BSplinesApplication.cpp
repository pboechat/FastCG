#include "BSplinesApplication.h"
#include <FontRegistry.h>
#include <Camera.h>
#include <KeyCode.h>
#include <MathT.h>

#include <algorithm>
#include <sstream>

const unsigned int BSplinesApplication::NUM_BSPLINE_SAMPLES = 100;
const glm::vec4 BSplinesApplication::BSPLINE_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 BSplinesApplication::KNOT_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 BSplinesApplication::CONTROL_POLYGON_COLOR(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 BSplinesApplication::CONTROL_POINT_COLOR(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 BSplinesApplication::SELECTED_CONTROL_POINT_COLOR(0.0f, 1.0f, 1.0f, 1.0f);
const float BSplinesApplication::CONTROL_POINT_SIZE = 7.5f;
const float BSplinesApplication::KNOT_SIZE = 3.5f;
const glm::vec4 BSplinesApplication::CONTROL_POINT_SELECTION_AREA(-10.0f, -10.0f, 10.0f, 10.0f);

BSplinesApplication::BSplinesApplication() :
	Application("bsplines", 800, 600),
	mState(BSPLINE_DEFINITION),
	mSelectedControlPointIndex(-1),
	mBSplineDegree(BSplineCurve::MINIMUM_DEGREE)
{
	mMainCameraPtr = new Camera(0.0f, 1.0f, -1.0f, 0.0f, (float) GetScreenHeight(), 0.0f, (float) GetScreenWidth(), PM_ORTHOGRAPHIC);
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

BSplinesApplication::~BSplinesApplication()
{
}

void BSplinesApplication::BeforeUpdate()
{
	char buffer[128];
	sprintf(buffer, "Degree: %d", mBSplineDegree);

#ifdef USE_PROGRAMMABLE_PIPELINE
	int top = 20;
	int heightIncrement = FontRegistry::STANDARD_FONT_SIZE + 8;
	DrawText("Press 'Enter' to plot B-Spline", FontRegistry::STANDARD_FONT_SIZE, 20, top , mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	DrawText("Press 'Backspace' to reset B-Spline", FontRegistry::STANDARD_FONT_SIZE, 20, top, mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	DrawText("Right mouse-click to add control point", FontRegistry::STANDARD_FONT_SIZE, 20, top, mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	DrawText("Left mouse-click to remove control point", FontRegistry::STANDARD_FONT_SIZE, 20, top, mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	DrawText("Press '+' and '-' to increase/decrease degree", FontRegistry::STANDARD_FONT_SIZE, 20, top, mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	DrawText(buffer, FontRegistry::STANDARD_FONT_SIZE, 20, top, mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
#else
	DrawText("Press 'Enter' to plot B-Spline", 12, 20, 20, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	DrawText("Press 'Backspace' to reset B-Spline", 12, 20, 35, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	DrawText("Right mouse-click to add control point", 12, 20, 50, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	DrawText("Left mouse-click to remove control point", 12, 20, 65, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	DrawText("Press '+' and '-' to increase/decrease degree", 12, 20, 90, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	DrawText(buffer, 12, 20, 105, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
#endif
}

void BSplinesApplication::OnMouseButton(int button, int state, int x, int y)
{
	if (state != 0)
	{
		return;
	}

	if (mState == BSPLINE_DEFINITION)
	{
		if (button == 0)
		{
			glm::vec3 point((float)x, (float)GetScreenHeight() - y, 0);

			int controlPointIndex = GetControlPointIndex(point);

			if (mSelectedControlPointIndex == -1)
			{
				mSelectedControlPointIndex = controlPointIndex;
				if (mSelectedControlPointIndex == -1) {
					mBSplineControlPoints.push_back(glm::vec2(point.x, point.y));
				} 
			
				if (mBSplineControlPoints.size() > 1)
				{
					UpdateControlPoints();
				}
			}
			else
			{
				if (controlPointIndex == -1)
				{
					mBSplineControlPoints[mSelectedControlPointIndex] = glm::vec2(point.x, point.y);
					mSelectedControlPointIndex = -1;
				}
				else 
				{
					mSelectedControlPointIndex = controlPointIndex;
				}
				UpdateControlPoints();
			}
		}
		else if (button == 2)
		{
			RemoveControlPoint();
		}
	}
}

void BSplinesApplication::OnKeyPress(int key)
{
	if (key == KeyCode::ESCAPE)
	{
		Quit();
		return;
	}

	if (mState == BSPLINE_DEFINITION)
	{
		if (key == KeyCode::PLUS)
		{
			mBSplineDegree = MathUI::Min(mBSplineDegree + 1, mBSplineControlPoints.size() - 1);
		}
		else if (key == KeyCode::MINUS)
		{
			mBSplineDegree = MathUI::Max(mBSplineDegree - 1, BSplineCurve::MINIMUM_DEGREE);
		}
		else if (key == KeyCode::RETURN)
		{
			SetState(BSPLINE_MANIPULATION);
		}
	}
	else
	{
		if (key == KeyCode::BACKSPACE)
		{
			SetState(BSPLINE_DEFINITION);
		}
	}
}

void BSplinesApplication::CreateBSpline() 
{
	std::vector<glm::vec3> vertices;

	mBSplinePtr = BSplineCurve::CreateUniform(mBSplineDegree, mBSplineControlPoints, true);

	glm::vec2 start = mBSplineControlPoints[0];
	float increment = 1.0f / NUM_BSPLINE_SAMPLES;
	float u = 0.0f;
	while (u <= 1.0f)
	{
		glm::vec2 point = mBSplinePtr->GetValue(u);
		vertices.push_back(glm::vec3(point.x, point.y, 0.0f));
		u += increment;
	}

	mBSplineLineStripPtr = new LineStrip(vertices, BSPLINE_COLOR);
	AddDrawable(mBSplineLineStripPtr);

	const std::vector<float>& rKnots = mBSplinePtr->GetKnots();
	vertices.clear();

	for (unsigned int i = 0; i < rKnots.size(); i++)
	{
		u = rKnots[i];
		glm::vec2 point = mBSplinePtr->GetValue(u);
		vertices.push_back(glm::vec3(point.x, point.y, 0.0f));
	}

	mBSplineKnotsPtr = new Points(vertices, KNOT_SIZE, KNOT_COLOR);
	AddDrawable(mBSplineKnotsPtr);
}

void BSplinesApplication::SetState(State state)
{
	if (mState == state)
	{
		return;
	}

	mState = state;

	if (mState == BSPLINE_DEFINITION)
	{
		RemoveBSpline();

	}
	else
	{
		CreateBSpline();
	}
}

void BSplinesApplication::RemoveControlPoint()
{
	if (mBSplineControlPoints.size() == 0)
	{
		return;
	}

	mBSplineControlPoints.erase(--mBSplineControlPoints.end());
	mSelectedControlPointIndex = -1;

	mBSplineDegree = MathUI::Max(MathUI::Min(mBSplineDegree, mBSplineControlPoints.size() - 1), BSplineCurve::MINIMUM_DEGREE);

	if (mBSplineControlPoints.size() > 1)
	{
		UpdateControlPoints();
	}
	else if (mBSplineControlPolygonPtr != 0)
	{
		mBSplineControlPoints.clear();
		RemoveDrawable(mBSplineControlPolygonPtr);
		mBSplineControlPolygonPtr = 0;
		RemoveDrawable(mBSplineControlPointsPtr);
		mBSplineControlPointsPtr = 0;
	}
}

void BSplinesApplication::UpdateControlPoints()
{
	if (mBSplineControlPolygonPtr != 0)
	{
		RemoveDrawable(mBSplineControlPolygonPtr);
		mBSplineControlPolygonPtr = 0;
	}

	if (mBSplineControlPointsPtr != 0)
	{
		RemoveDrawable(mBSplineControlPointsPtr);
		mBSplineControlPointsPtr = 0;
	}

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;

	for (unsigned int i = 0; i < mBSplineControlPoints.size(); i++)
	{
		glm::vec2 controlPoint = mBSplineControlPoints[i];
		vertices.push_back(glm::vec3(controlPoint.x, controlPoint.y, 0.0f));

		if (i == mSelectedControlPointIndex) 
		{
			colors.push_back(SELECTED_CONTROL_POINT_COLOR);
		}
		else
		{
			colors.push_back(CONTROL_POINT_COLOR);
		}
	}

	mBSplineControlPolygonPtr = new LineStrip(vertices, CONTROL_POLYGON_COLOR);
	AddDrawable(mBSplineControlPolygonPtr);

	mBSplineControlPointsPtr = new Points(vertices, CONTROL_POINT_SIZE, colors);
	AddDrawable(mBSplineControlPointsPtr);
}

int BSplinesApplication::GetControlPointIndex(const glm::vec3& rPoint)
{
	for (unsigned int i = 0; i < mBSplineControlPoints.size(); i++)
	{
		glm::vec3 controlPoint(mBSplineControlPoints[i], 0.0f);

		if ((rPoint.x >= controlPoint.x + CONTROL_POINT_SELECTION_AREA.x && rPoint.x <= controlPoint.x + CONTROL_POINT_SELECTION_AREA.z) &&
			(rPoint.y >= controlPoint.y + CONTROL_POINT_SELECTION_AREA.y && rPoint.y <= controlPoint.y + CONTROL_POINT_SELECTION_AREA.w))
		{
			return i;
		}
	}

	return -1;
}

void BSplinesApplication::RemoveBSpline()
{
	RemoveDrawable(mBSplineLineStripPtr);
	RemoveDrawable(mBSplineKnotsPtr);
	mBSplineLineStripPtr = 0;
	mBSplineKnotsPtr = 0;
	mBSplinePtr = 0;
}
