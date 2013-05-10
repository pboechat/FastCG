#if (!defined(FONT_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define FONT_H_

#include <Pointer.h>
#include <Shader.h>
#include <Material.h>
#include <Mesh.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>

class Font
{
public:
	Font(const std::string& rFileName, unsigned int size);
	~Font();

	void DrawText(const std::string& rText, unsigned int size, int x, int y, const glm::vec4& rColor);

private:
	static const unsigned int NUM_CHARS;

	std::string mFileName;
	unsigned int mSize;
	ShaderPtr mFontShaderPtr;
	std::vector<MeshPtr> mBillboards;
	std::vector<unsigned int> mCharactersTexturesIds;
	std::vector<int> mSpacings;
	std::vector<glm::vec2> mOffsets;

	void AllocateResources();
	void DeallocateResources();

	MeshPtr CreateCharacterBillboard(unsigned int width, unsigned int height, float s, float t);

};

typedef Pointer<Font> FontPtr;

#endif