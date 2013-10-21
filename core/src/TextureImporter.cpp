#include <TextureImporter.h>
#include <Exception.h>

#include <GL3/gl3w.h>
#include <IL/il.h>

bool TextureImporter::s_mInitialized = false;

void TextureImporter::Initialize()
{
	if (s_mInitialized)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "s_mInitialized");
	}

	ilInit();
	s_mInitialized = true;
}

void TextureImporter::Dispose()
{
	s_mInitialized = false;
}

Texture* TextureImporter::Import(const std::string& rFilePath)
{
	if (!s_mInitialized)
	{
		THROW_EXCEPTION(Exception, "Trying to load texture from disk before initialization");
	}

	unsigned int imageId;

	ilGenImages(1, &imageId);
	ilBindImage(imageId);

	if (!ilLoadImage(rFilePath.c_str()))
	{
		THROW_EXCEPTION(Exception, "Error loading image: %s", rFilePath.c_str());
	}

	unsigned int width = (unsigned int)ilGetInteger(IL_IMAGE_WIDTH);
	unsigned int height = (unsigned int)ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned int bytesPerPixel = (unsigned int)ilGetInteger(IL_IMAGE_BPP);
	unsigned int internalFormat = (unsigned int)ilGetInteger(IL_IMAGE_FORMAT);
	unsigned char* pData = ilGetData();

	Texture* pTexture = new Texture(width, height, bytesPerPixel, internalFormat, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT, (void*)pData);

	ilDeleteImages(1, &imageId);

	return pTexture;
}

