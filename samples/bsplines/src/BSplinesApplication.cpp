#include "BSplinesApplication.h"
#include <Camera.h>
#include <KeyCode.h>
#include <MathT.h>

#include <algorithm>
#include <sstream>

const unsigned int BSplinesApplication::MIN_DEGREE = 1;
const unsigned int BSplinesApplication::MAX_DEGREE = 10;
const unsigned int BSplinesApplication::NUM_BSPLINE_SAMPLES = 100;
const glm::vec4 BSplinesApplication::BSPLINE_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 BSplinesApplication::CONTROL_POLYGON_COLOR(0.0f, 0.0f, 1.0f, 1.0f);

BSplinesApplication::BSplinesApplication() :
	Application("bsplines", 800, 600),
	mState(BSPLINE_DEFINITION),
	mBSplineDegree(MIN_DEGREE)
{
	mMainCameraPtr = new Camera(0.0f, 1.0f, -1.0f, 0.0f, (float) GetScreenHeight(), 0.0f, (float) GetScreenWidth(), PM_ORTHOGRAPHIC);
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

BSplinesApplication::~BSplinesApplication()
{
}

void BSplinesApplication::OnStart()
{
}

void BSplinesApplication::BeforeDisplay()
{
	char buffer[128];
	sprintf(buffer, "Degree: %d", mBSplineDegree);
	DrawText(buffer, 12, 20, 20, mStandardFontPtr, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
			AddControlPoint(glm::vec3((float)x, (float)GetScreenHeight() - y, 0));
		}
		else if (button == 2)
		{
			RemoveControlPoint();
		}
	}
}

void BSplinesApplication::OnKeyPress(int key)
{
	if (mState == BSPLINE_DEFINITION)
	{
		if (key == KeyCode::PLUS)
		{
			mBSplineDegree = MathUI::Min(mBSplineDegree + 1, MAX_DEGREE);
		}
		else if (key == KeyCode::HYPHEN)
		{
			mBSplineDegree = MathUI::Max(mBSplineDegree - 1, MIN_DEGREE);
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

void BSplinesApplication::SetState(State state)
{
	if (mState == state)
	{
		return;
	}

	mState = state;

	if (mState == BSPLINE_DEFINITION)
	{
		RemoveDrawable(mBSplineCurvePtr);
	}
	else
	{
		std::vector<glm::vec3> vertices;

		mBSplinePtr = BSpline::CreateUniform(mBSplineDegree, mBSplineControlPoints, true);
		glm::vec2 start = mBSplineControlPoints[0];
		float increment = 1.0f / NUM_BSPLINE_SAMPLES;
		float u = 0.0f;
		while (u < 1.0f)
		{
			glm::vec2 point = mBSplinePtr->GetValue(u);
			vertices.push_back(glm::vec3(point.x, point.y, 0.0f));
			u += increment;
		}

		mBSplineCurvePtr = new LineStrip(vertices, BSPLINE_COLOR);

		AddDrawable(mBSplineCurvePtr);
	}
}

void BSplinesApplication::AddControlPoint(const glm::vec3& rPoint)
{
	mBSplineControlPoints.push_back(glm::vec2(rPoint.x, rPoint.y));

	if (mBSplineControlPoints.size() > 1)
	{
		UpdateBSplineControlPointsLineStrip();
	}
}

void BSplinesApplication::RemoveControlPoint()
{
	if (mBSplineControlPoints.size() == 0)
	{
		return;
	}

	mBSplineControlPoints.erase(--mBSplineControlPoints.end());

	if (mBSplineControlPoints.size() > 1)
	{
		UpdateBSplineControlPointsLineStrip();
	}
	else if (mControlPolygonPtr != 0)
	{
		RemoveDrawable(mControlPolygonPtr);
		mControlPolygonPtr = 0;
	}
}

void BSplinesApplication::UpdateBSplineControlPointsLineStrip()
{
	if (mControlPolygonPtr != 0)
	{
		RemoveDrawable(mControlPolygonPtr);
		mControlPolygonPtr = 0;
	}

	std::vector<glm::vec3> vertices;

	for (unsigned int i = 0; i < mBSplineControlPoints.size(); i++)
	{
		glm::vec2 controlPoint = mBSplineControlPoints[i];
		vertices.push_back(glm::vec3(controlPoint.x, controlPoint.y, 0.0f));
	}

	mControlPolygonPtr = new LineStrip(vertices, CONTROL_POLYGON_COLOR);

	AddDrawable(mControlPolygonPtr);
}
