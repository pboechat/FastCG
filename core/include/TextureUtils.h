#ifndef TEXTUREUTILS_H_
#define TEXTUREUTILS_H_

#include <Texture.h>

#include <string>

class TextureUtils
{
public:
	static Texture* LoadPNGAsTexture(const std::string& rFileName);

private:
	TextureUtils()
	{
	}

	~TextureUtils()
	{
	}

};

#endif