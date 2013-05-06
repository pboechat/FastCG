#if (!defined(FONT_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define FONT_H_

#include <Pointer.h>
#include <Shader.h>
#include <Material.h>
#include <TriangleMesh.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>

class Font
{
public:
	Font(const std::string& rFileName, unsigned int size);
	~Font();

	void DrawText(const std::string& rText, unsigned int size, unsigned int x, unsigned int y, const glm::vec4& rColor);

private:
	static const unsigned int NUM_CHARS;

	std::string mFileName;
	unsigned int mSize;
	ShaderPtr mFontShaderPtr;
	std::vector<TriangleMeshPtr> mBillboards;
	std::vector<unsigned int> mCharactersTexturesIds;
	std::vector<unsigned int> mSpacings;
	std::vector<glm::vec2> mOffsets;

	void AllocateResources();
	void DeallocateResources();

	TriangleMeshPtr CreateCharacterBillboard(unsigned int width, unsigned int height, float s, float t);

};

typedef Pointer<Font> FontPtr;

#endif