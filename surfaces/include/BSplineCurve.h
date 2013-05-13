#ifndef BSPLINECURVE_H_
#define BSPLINECURVE_H_

#include <glm/glm.hpp>

#include <vector>

class BSplineCurve
{
public:
	static const unsigned int MINIMUM_DEGREE;
	static BSplineCurve* CreateUniform(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, bool clamped);

	inline const std::vector<float>& GetKnots() const
	{
		return mKnots;
	}

	inline unsigned int GetDegree() const
	{
		return mDegree;
	}

	glm::vec2 GetValue(float x) const;

private:
	unsigned int mDegree;
	std::vector<float> mKnots;
	std::vector<glm::vec2> mControlPoints;

	BSplineCurve(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, const std::vector<float>& rKnots);

	float DeBoors(int n, int i, float x) const;

};

#endif