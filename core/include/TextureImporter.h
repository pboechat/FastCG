#ifndef TEXTUREIMPORTER_H_
#define TEXTUREIMPORTER_H_

#include <Texture.h>

#include <string>

class TextureImporter
{
public:
	static void Initialize();
	static Texture* Import(const std::string& rFilePath);
	static void Dispose();

private:
	static bool s_mInitialized;

	static InternalFormat GetInternalFormat(int format);

	TextureImporter()
	{
	}

	~TextureImporter()
	{
	}

};

#endif