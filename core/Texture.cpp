#include "include/Texture.h"
#include "include/Exception.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glext.h>

Texture::Texture(unsigned int width, unsigned int height, TextureFormat format, FilterMode filter, WrapMode wrapMode)
{
	Allocate(width, height, format, filter, wrapMode);
}

Texture::Texture(unsigned int width, unsigned int height, TextureFormat format, FilterMode filter, WrapMode wrapMode, float* pData)
{
	Allocate(width, height, format, filter, wrapMode, pData);
}

Texture::~Texture()
{
	Deallocate();
}

void Texture::Allocate(unsigned int width, unsigned int height, TextureFormat format, FilterMode filter, WrapMode wrapMode, float* pData)
{
	mWidth = width;
	mHeight = height;
	mFormat = format;
	mFilter = filter;
	mWrapMode = wrapMode;
	mpData = pData;

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	SetUpFilter();
	SetUpWrapping();

	glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormatMapping(), width, height, 0, GetGLFormatMapping(), GL_FLOAT, pData);

	if (mFilter == FM_TRILINEAR)
	{
		GenerateMipmaps();
	}

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// TODO: check allocation errors!
}

void Texture::Deallocate()
{
	glDeleteTextures(1, &mTextureId);
}

void Texture::SetUpFilter() const
{
	switch (mFilter)
	{
	case FM_POINT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case FM_BILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case FM_TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown filter mode: %d", mFilter);
	}
}

void Texture::SetUpWrapping() const
{
	switch (mWrapMode)
	{
	case WM_CLAMP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case WM_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown wrapping mode: %d", mWrapMode);
	}
}

unsigned int Texture::GetGLInternalFormatMapping() const
{
	switch (mFormat)
	{
	case TF_RGB:
		return GL_RGB;
	case TF_RGBA:
		return GL_RGBA;
	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown texture format: %d", mFormat);
	}
}

unsigned int Texture::GetGLFormatMapping() const
{
	switch (mFormat)
	{
	case TF_RGB:
		return GL_RGB;
	case TF_RGBA:
		return GL_RGBA;
	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown texture format: %d", mFormat);
	}
}

void Texture::GenerateMipmaps() const
{
	glGenerateMipmap(GL_TEXTURE_2D);
}
