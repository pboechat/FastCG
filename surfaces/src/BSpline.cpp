#include <BSpline.h>
#include <Exception.h>

Pointer<BSpline> BSpline::CreateUniform(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, bool clamped)
{
	unsigned int n = rControlPoints.size() - 1;
	unsigned int m = n * degree + 1;

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
	float knot = increment;
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
	glm::vec2 point(0.0f, 0.0f);
	for (unsigned int i = 0; i < mControlPoints.size(); i++)
	{
		float d = DeBoors(mDegree, i, x);
		point += d * mControlPoints[i];
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
		int l = FindKnotSpanLowerBound(x);
		return (l >= i && l <= (i + 1)) ? 1.0f : 0.0f;
	}

	float a = (mKnots[i + n] - mKnots[i]);
	float c1;
	if (a == 0)
	{
		c1 = 0;
	}
	else 
	{
		c1 = (x - mKnots[i]) / a;
	}

	float b = (mKnots[i + n + 1] - mKnots[i + 1]);
	float c2;
	if (b == 0)
	{
		c2 = 0;
	}
	else 
	{
		c2 = (mKnots[i + n + 1] - x) / b;
	}

	return c1 * DeBoors(n - 1, i, x) + c2 * DeBoors(n - 1, i + 1, x);
}

