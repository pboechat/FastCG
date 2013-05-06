#ifndef BSPLINE_H_
#define BSPLINE_H_

#include <glm/glm.hpp>

#include <vector>

class BSpline
{
public:
	BSpline(int degree, const std::vector<float>& rKnots, const std::vector<glm::vec2>& rControlPoints);

	float Get(float x) const;

private:
	int mDegree;
	std::vector<float> mKnots;
	std::vector<glm::vec2> mControlPoints;

	unsigned int FindUpperKnotBound(float x) const;

	float DeBoors(int k, int i, float x) const;

};

#endif