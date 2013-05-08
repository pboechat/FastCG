#ifndef BSPLINE_H_
#define BSPLINE_H_

#include <Pointer.h>
#include <glm/glm.hpp>

#include <vector>

class BSpline
{
public:
	static const unsigned int MINIMUM_DEGREE;
	static Pointer<BSpline> CreateUniform(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, bool clamped);

	unsigned int GetDegree() const;
	glm::vec2 GetValue(float x) const;

private:
	unsigned int mDegree;
	std::vector<float> mKnots;
	std::vector<glm::vec2> mControlPoints;

	BSpline(unsigned int degree, const std::vector<glm::vec2>& rControlPoints, const std::vector<float>& rKnots);

	unsigned int FindKnotSpanLowerBound(float x) const;
	float DeBoors(int n, int i, float x) const;

};

typedef Pointer<BSpline> BSplinePtr;

#endif