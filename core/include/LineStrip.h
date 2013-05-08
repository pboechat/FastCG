#ifndef LINESTRIP_H_
#define LINESTRIP_H_

#include <Transformable.h>
#include <Drawable.h>
#include <Pointer.h>
#include <Material.h>

#include <glm/glm.hpp>

#include <vector>

class LineStrip : public Transformable, public Drawable
{
public:
	LineStrip(const std::vector<glm::vec3>& rVertices, const glm::vec4& rColor);
	LineStrip(const std::vector<glm::vec3>& rVertices, const std::vector<glm::vec4>& rColors);
	~LineStrip();

	virtual void Draw();

private:
	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec4> mColors;
#ifdef USE_PROGRAMMABLE_PIPELINE
	MaterialPtr mLineStripMaterialPtr;
	unsigned int mLineStripVAOId;
	unsigned int mVerticesVBOId;
	unsigned int mColorsVBOId;
#else
	unsigned int mDisplayListId;
#endif

	void AllocateResources();
	void DeallocateResources();

};

typedef Pointer<LineStrip> LineStripPtr;

#endif