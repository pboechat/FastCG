#include <TextureImporter.h>
#include <Exception.h>

#include <Windows.h>
#include <GL/gl.h>
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
	InternalFormat internalFormat = GetInternalFormat(ilGetInteger(IL_IMAGE_FORMAT));
	unsigned char* pData = ilGetData();

	Texture* pTexture = new Texture(width, height, bytesPerPixel, internalFormat, DF_UNSIGNED_CHAR, FM_LINEAR_FILTER, WM_REPEAT, (void*)pData);

	ilDeleteImages(1, &imageId);

	return pTexture;
}

InternalFormat TextureImporter::GetInternalFormat(int format)
{
	switch (format)
	{
	case GL_RGB:
		return IF_RGB;

	case GL_RGBA:
		return IF_RGBA;

	case GL_LUMINANCE_ALPHA:
		return IF_LUMINANCE_ALPHA;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unsupported internal format: %d", format);
	}
}
