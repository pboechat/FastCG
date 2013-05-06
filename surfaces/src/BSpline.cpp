#include <BSpline.h>

BSpline::BSpline(int degree, const std::vector<float>& rKnots, const std::vector<glm::vec2>& rControlPoints) :
	mDegree(degree), mKnots(rKnots), mControlPoints(rControlPoints)
{
}

float BSpline::Get(float x) const
{
	return DeBoors(mDegree, FindUpperKnotBound(x) - 1, x);
}

unsigned int BSpline::FindUpperKnotBound(float x) const
{
	if (x < mKnots[0])
	{
		return 0;
	}

	for (unsigned int i = 1; i < mKnots.size(); i++)
	{
		if (x < mKnots[i])
		{
			return i;
		}
	}

	return mKnots.size();
}

float BSpline::DeBoors(int k, int i, float x) const
{
	if (k == 0)
	{
		return mKnots[i];
	} 
	else 
	{
		float alpha = (x - mKnots[i]) / (mKnots[i + mDegree + 1 - k] - mKnots[i]);
		return (1 - alpha) * DeBoors(k - 1, i - 1, x) + alpha * DeBoors(k - 1, i, x);
	}
}