#include "BSplinesEditor.h"

#include <Application.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <MathT.h>
#include <LineRenderer.h>
#include <PointsRenderer.h>

#include <algorithm>
#include <sstream>

const unsigned int BSplinesEditor::NUM_BSPLINE_SAMPLES = 100;
const glm::vec4 BSplinesEditor::BSPLINE_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 BSplinesEditor::KNOT_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 BSplinesEditor::CONTROL_POLYGON_COLOR(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 BSplinesEditor::CONTROL_POINT_COLOR(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 BSplinesEditor::SELECTED_CONTROL_POINT_COLOR(0.0f, 1.0f, 1.0f, 1.0f);
const float BSplinesEditor::CONTROL_POINT_SIZE = 7.5f;
const float BSplinesEditor::KNOT_SIZE = 3.5f;
const glm::vec4 BSplinesEditor::CONTROL_POINT_SELECTION_AREA(-10.0f, -10.0f, 10.0f, 10.0f);

COMPONENT_IMPLEMENTATION(BSplinesEditor, Behaviour);

void BSplinesEditor::OnInstantiate()
{
	mpBSplineCurve = 0;
	mpBSpline = 0;
	mpKnots = 0;
	mpControlPolygon = 0;
	mpControlPoints = 0;
	mState = BSPLINE_DEFINITION;
	mSelectedControlPointIndex = -1;
	mBSplineDegree = BSplineCurve::MINIMUM_DEGREE;
	mLastKeyPressTime = 0;
	mLastRightMouseButtonClickTime = 0;
	mLastLeftMouseButtonClickTime = 0;
}

void BSplinesEditor::OnDestroy()
{
	if (mpBSpline != 0)
	{
		delete mpBSpline;
	}

	if (mpKnots != 0)
	{
		delete mpKnots;
	}

	if (mpControlPolygon != 0)
	{
		delete mpControlPolygon;
	}

	if (mpControlPoints != 0)
	{
		delete mpControlPoints;
	}
}

void BSplinesEditor::OnUpdate( float time, float deltaTime )
{
	if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}

	if (mState == BSPLINE_DEFINITION)
	{
		if (Input::GetKey(KeyCode::PLUS) && time - mLastKeyPressTime > 0.333f)
		{
			mBSplineDegree = MathUI::Min(mBSplineDegree + 1, mControlPoints.size() - 1);
			mLastKeyPressTime = time;
		}

		else if (Input::GetKey(KeyCode::MINUS) && time - mLastKeyPressTime > 0.333f)
		{
			mBSplineDegree = MathUI::Max(mBSplineDegree - 1, BSplineCurve::MINIMUM_DEGREE);
			mLastKeyPressTime = time;
		}

		else if (Input::GetKey(KeyCode::RETURN) && time - mLastKeyPressTime > 0.333f)
		{
			SetState(BSPLINE_MANIPULATION);
			mLastKeyPressTime = time;
		}

		if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftMouseButtonClickTime > 0.333f)
		{
			glm::vec3 point = glm::vec3(Input::GetMousePosition(), 0.0f);
			int controlPointIndex = GetControlPointIndex(point);

			if (mSelectedControlPointIndex == -1)
			{
				mSelectedControlPointIndex = controlPointIndex;

				if (mSelectedControlPointIndex == -1)
				{
					mControlPoints.push_back(glm::vec2(point.x, point.y));
				}

				if (mControlPoints.size() > 1)
				{
					UpdateControlPoints();
				}
			}

			else
			{
				if (controlPointIndex == -1)
				{
					mControlPoints[mSelectedControlPointIndex] = glm::vec2(point.x, point.y);
					mSelectedControlPointIndex = -1;
				}

				else
				{
					mSelectedControlPointIndex = controlPointIndex;
				}

				UpdateControlPoints();
			}

			mLastLeftMouseButtonClickTime = time;
		}

		else if (Input::GetMouseButton(MouseButton::RIGHT_BUTTON) && time - mLastRightMouseButtonClickTime > 0.333f)
		{
			RemoveLastControlPoint();
			mLastRightMouseButtonClickTime = time;
		}
	}

	else
	{
		if (Input::GetKey(KeyCode::BACKSPACE))
		{
			SetState(BSPLINE_DEFINITION);
		}
	}

	DrawGUI();
}

void BSplinesEditor::SetState(State state)
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
		UpdateBSpline();
	}
}

void BSplinesEditor::RemoveLastControlPoint()
{
	if (mControlPoints.size() == 0)
	{
		return;
	}

	mControlPoints.erase(--mControlPoints.end());
	mSelectedControlPointIndex = -1;
	mBSplineDegree = MathUI::Max(MathUI::Min(mBSplineDegree, mControlPoints.size() - 1), BSplineCurve::MINIMUM_DEGREE);

	if (mControlPoints.size() > 1)
	{
		UpdateControlPoints();
	}

	else
	{
		RemoveControlPoints();
	}
}

void BSplinesEditor::UpdateBSpline()
{
	std::vector<glm::vec3> vertices;

	if (mpBSplineCurve != 0)
	{
		delete mpBSplineCurve;
	}
	mpBSplineCurve = BSplineCurve::CreateUniform(mBSplineDegree, mControlPoints, true);

	glm::vec2 start = mControlPoints[0];
	float increment = 1.0f / NUM_BSPLINE_SAMPLES;
	float u = 0.0f;

	while (u <= 1.0f)
	{
		glm::vec2 point = mpBSplineCurve->GetValue(u);
		vertices.push_back(glm::vec3(point.x, point.y, 0.0f));
		u += increment;
	}

	if (mpBSpline != 0)
	{
		delete mpBSpline;
	}
	mpBSpline = new LineStrip(vertices, BSPLINE_COLOR);

	LineRenderer* pLineRenderer = dynamic_cast<LineRenderer*>(mpBSplineGameObject->GetComponent(LineRenderer::TYPE));
	pLineRenderer->SetLineStrip(mpBSpline);
	mpBSplineGameObject->SetActive(true);

	const std::vector<float>& rKnots = mpBSplineCurve->GetKnots();
	vertices.clear();

	for (unsigned int i = 0; i < rKnots.size(); i++)
	{
		u = rKnots[i];
		glm::vec2 point = mpBSplineCurve->GetValue(u);
		vertices.push_back(glm::vec3(point.x, point.y, 0.0f));
	}

	if (mpKnots != 0)
	{
		delete mpKnots;
	}
	mpKnots = new Points(vertices, KNOT_SIZE, KNOT_COLOR);

	PointsRenderer* pPointsRenderer = dynamic_cast<PointsRenderer*>(mpKnotsGameObject->GetComponent(PointsRenderer::TYPE));
	pPointsRenderer->SetPoints(mpKnots);
	mpKnotsGameObject->SetActive(true);
}

void BSplinesEditor::RemoveBSpline()
{
	mpBSplineGameObject->SetActive(false);
	mpKnotsGameObject->SetActive(false);
	mpBSplineCurve = 0;
}

void BSplinesEditor::UpdateControlPoints()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;

	for (unsigned int i = 0; i < mControlPoints.size(); i++)
	{
		const glm::vec2& controlPoint = mControlPoints[i];
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

	if (mpControlPolygon != 0)
	{
		delete mpControlPolygon;
	}
	mpControlPolygon = new LineStrip(vertices, CONTROL_POLYGON_COLOR);

	LineRenderer* pLineRenderer = dynamic_cast<LineRenderer*>(mpControlPolygonGameObject->GetComponent(LineRenderer::TYPE));
	pLineRenderer->SetLineStrip(mpControlPolygon);
	mpControlPolygonGameObject->SetActive(true);

	if (mpControlPoints != 0)
	{
		delete mpControlPoints;
	}
	mpControlPoints = new Points(vertices, CONTROL_POINT_SIZE, colors);

	PointsRenderer* pPointsRenderer = dynamic_cast<PointsRenderer*>(mpControlPointsGameObject->GetComponent(PointsRenderer::TYPE));
	pPointsRenderer->SetPoints(mpControlPoints);
	mpControlPointsGameObject->SetActive(true);
}

void BSplinesEditor::RemoveControlPoints()
{
	mControlPoints.clear();
	mpControlPolygonGameObject->SetActive(false);
	mpControlPointsGameObject->SetActive(false);
}

int BSplinesEditor::GetControlPointIndex(const glm::vec3& rPoint)
{
	for (unsigned int i = 0; i < mControlPoints.size(); i++)
	{
		const glm::vec2& controlPoint = mControlPoints[i];

		if ((rPoint.x >= controlPoint.x + CONTROL_POINT_SELECTION_AREA.x && rPoint.x <= controlPoint.x + CONTROL_POINT_SELECTION_AREA.z) &&
				(rPoint.y >= controlPoint.y + CONTROL_POINT_SELECTION_AREA.y && rPoint.y <= controlPoint.y + CONTROL_POINT_SELECTION_AREA.w))
		{
			return i;
		}
	}

	return -1;
}

void BSplinesEditor::DrawGUI()
{
	char buffer[128];
	sprintf(buffer, "Degree: %d", mBSplineDegree);
#ifdef USE_PROGRAMMABLE_PIPELINE
	int top = 20;
	int heightIncrement = FontRegistry::STANDARD_FONT_SIZE + 8;
	Application::GetInstance()->DrawText("Press 'Enter' to plot B-Spline", FontRegistry::STANDARD_FONT_SIZE, 20, top , glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	Application::GetInstance()->DrawText("Press 'Backspace' to reset B-Spline", FontRegistry::STANDARD_FONT_SIZE, 20, top, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	Application::GetInstance()->DrawText("Right mouse-click to add control point", FontRegistry::STANDARD_FONT_SIZE, 20, top, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	Application::GetInstance()->DrawText("Left mouse-click to remove control point", FontRegistry::STANDARD_FONT_SIZE, 20, top, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	Application::GetInstance()->DrawText("Press '+' and '-' to increase/decrease degree", FontRegistry::STANDARD_FONT_SIZE, 20, top, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	top += heightIncrement;
	Application::GetInstance()->DrawText(buffer, FontRegistry::STANDARD_FONT_SIZE, 20, top, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
#else
	Application::GetInstance()->DrawText("Press 'Enter' to plot B-Spline", 12, 20, 20, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press 'Backspace' to reset B-Spline", 12, 20, 35, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Right mouse-click to add control point", 12, 20, 50, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Left mouse-click to remove control point", 12, 20, 65, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press '+' and '-' to increase/decrease degree", 12, 20, 90, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText(buffer, 12, 20, 105, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
#endif
}