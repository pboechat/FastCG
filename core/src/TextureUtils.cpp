#include <TextureUtils.h>
#include <PNGLoader.h>

Texture* TextureUtils::LoadPNGAsTexture(const std::string& rFileName)
{
	unsigned int width;
	unsigned int height;
	bool transparency;
	unsigned char* pData;
	PNGLoader::Load(rFileName, width, height, transparency, &pData);
	Texture* pTexture = new Texture(width, height, (transparency) ? TF_RGBA : TF_RGB, DT_UNSIGNED_CHAR, FM_BILINEAR, WM_REPEAT, pData);
	return pTexture;
}
