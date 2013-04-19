#include <Texture.h>
#include <Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

Texture::Texture(unsigned int width, unsigned int height, TextureFormat format, DataType dataType, FilterMode filter, WrapMode wrapMode, void* pData) :
	mWidth(width), mHeight(height), mFormat(format), mDataType(dataType), mFilter(filter), mWrapMode(wrapMode), mpData(pData)
{
	AllocateResources();
}

Texture::~Texture()
{
	DeallocateResources();
}

void Texture::AllocateResources()
{
	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	SetUpFilter();
	SetUpWrapping();

	if (mDataType == DT_UNSIGNED_CHAR)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormatMapping(), mWidth, mHeight, 0, GetFormatMapping(), GetDataTypeMapping(), mpData);

	if (mFilter == FM_TRILINEAR)
	{
		GenerateMipmaps();
	}

	// TODO: check allocation errors!
}

void Texture::DeallocateResources()
{
	glDeleteTextures(1, &mTextureId);
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown filter mode: %d", mFilter)
		;
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
		THROW_EXCEPTION(Exception, "Unknown wrapping mode: %d", mWrapMode)
		;
	}
}

unsigned int Texture::GetInternalFormatMapping() const
{
	switch (mFormat)
	{
	case TF_RGB:
		return GL_RGB;

	case TF_RGBA:
		return GL_RGBA;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown texture format: %d", mFormat)
		;
		return 0;
	}
}

unsigned int Texture::GetFormatMapping() const
{
	switch (mFormat)
	{
	case TF_RGB:
		return GL_RGB;

	case TF_RGBA:
		return GL_RGBA;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown texture format: %d", mFormat)
		;
		return 0;
	}
}

unsigned int Texture::GetDataTypeMapping() const
{
	switch (mDataType)
	{
	case DT_FLOAT:
		return GL_FLOAT;

	case DT_UNSIGNED_CHAR:
		return GL_UNSIGNED_BYTE;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown data type: %d", mDataType)
		;
		return 0;
	}
}

void Texture::GenerateMipmaps() const
{
	glGenerateMipmap(GL_TEXTURE_2D);
}
