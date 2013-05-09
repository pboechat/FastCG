#ifndef POINTS_H_
#define POINTS_H_

#include <Transformable.h>
#include <Drawable.h>
#include <Pointer.h>
#include <Material.h>

#include <glm/glm.hpp>

#include <vector>

class Points : public Transformable, public Drawable
{
public:
	Points(const std::vector<glm::vec3>& rVertices, float size, const glm::vec4& rColor);
	Points(const std::vector<glm::vec3>& rVertices, float size, const std::vector<glm::vec4>& rColors);
	~Points();

protected:
	virtual void OnDraw();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec4> mColors;
	float mSize;
#ifdef USE_PROGRAMMABLE_PIPELINE
	MaterialPtr mPointsMaterialPtr;
	unsigned int mPointsVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mColorsVBOId;
#else
	unsigned int mDisplayListId;
#endif

	void AllocateResources();
	void DeallocateResources();

};

typedef Pointer<Points> PointsPtr;

#endif