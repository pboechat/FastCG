#include <BSpline.h>
#include <Exception.h>

const unsigned int BSpline::MINIMUM_DEGREE = 2;

Pointer<BSpline> BSpline::CreateUniform(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, bool clamped)
{
	if (degree < MINIMUM_DEGREE)
	{
		THROW_EXCEPTION(Exception, "Minimum degree is %d: %d", MINIMUM_DEGREE, degree);
	}

	if (degree > rControlPoints.size() - 1)
	{
		THROW_EXCEPTION(Exception, "Insufficient control points for degree %d: %d", degree, rControlPoints.size());
	}

	unsigned int n = rControlPoints.size() - 1;
	unsigned int m = n + degree + 1;
	std::vector<float> knots(m + 1);
	unsigned int start, end;

	if (clamped)
	{
		start = degree + 1;
		end = m - degree;
	}

	else
	{
		start = 1;
		end = m;
	}

	for (unsigned int i = 0; i < start; i++)
	{
		knots[i] = 0.0f;
	}

	float increment = 1.0f / m;
	float knot = start * increment;

	for (unsigned int i = start; i <= end; i++, knot += increment)
	{
		knots[i] = knot;
	}

	for (unsigned int i = end; i <= m; i++)
	{
		knots[i] = 1.0f;
	}

	return new BSpline(degree, rControlPoints, knots);
}

BSpline::BSpline(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, const std::vector<float>& rKnots) :
	mDegree(degree),
	mControlPoints(rControlPoints),
	mKnots(rKnots)
{
}

glm::vec2 BSpline::GetValue(float x) const
{
	glm::vec2 point;

	for (unsigned int i = 0; i < mControlPoints.size(); i++)
	{
		point += mControlPoints[i] * DeBoors(mDegree, i, x);
	}

	return point;
}

unsigned int BSpline::FindKnotSpanLowerBound(float x) const
{
	if (x < 0)
	{
		THROW_EXCEPTION(Exception, "Out of domain: %f", x);
	}

	for (unsigned int i = 0; i < mKnots.size(); i++)
	{
		if (x < mKnots[i])
		{
			return i - 1;
		}
	}

	THROW_EXCEPTION(Exception, "Out of domain: %f", x);
}

float BSpline::DeBoors(int n, int i, float x) const
{
	if (n == 0)
	{
		return (x >= mKnots[i] && x < mKnots[i + 1]) ? 1.0f : 0.0f;
	}

	float denominator = (mKnots[i + n] - mKnots[i]);
	float weight1;

	if (denominator == 0)
	{
		weight1 = 0;
	}

	else
	{
		weight1 = (x - mKnots[i]) / denominator;
	}

	denominator = (mKnots[i + n + 1] - mKnots[i + 1]);
	float weight2;

	if (denominator == 0)
	{
		weight2 = 0;
	}

	else
	{
		weight2 = (mKnots[i + n + 1] - x) / denominator;
	}

	return weight1 * DeBoors(n - 1, i, x) + weight2 * DeBoors(n - 1, i + 1, x);
}

