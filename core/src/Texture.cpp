#include <Texture.h>
#include <Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>

Texture::Texture(unsigned int width, unsigned int height, unsigned int bytesPerPixel, InternalFormat internalFormat, DataType dataType, FilterMode filter, WrapMode wrapMode, void* pData) :
	mTextureId(0),
	mWidth(width), 
	mHeight(height), 
	mBytesPerPixel(bytesPerPixel), 
	mInternalFormat(internalFormat), 
	mDataType(dataType), 
	mFilter(filter), 
	mWrapMode(wrapMode), 
	mpData(pData)
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

	if (mDataType == DF_UNSIGNED_CHAR)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, mBytesPerPixel, mWidth, mHeight, 0, GetInternalFormatMapping(), GetDataTypeMapping(), mpData);

	if (mFilter == FM_MIPMAPS)
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

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetUpFilter() const
{
	switch (mFilter)
	{
	case FM_POINT_FILTER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case FM_LINEAR_FILTER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case FM_MIPMAPS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

unsigned int Texture::GetInternalFormatMapping() const
{
	switch (mInternalFormat)
	{
	case IF_RGB:
		return GL_RGB;

	case IF_RGBA:
		return GL_RGBA;

	case IF_BGR:
		return GL_BGR;

	case IF_BGRA:
		return GL_BGRA;

	case IF_LUMINANCE_ALPHA:
		return GL_LUMINANCE_ALPHA;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown internal format: %d", mInternalFormat);
	}
}

unsigned int Texture::GetDataTypeMapping() const
{
	switch (mDataType)
	{
	case DF_FLOAT:
		return GL_FLOAT;

	case DF_UNSIGNED_CHAR:
		return GL_UNSIGNED_BYTE;

	default:
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown data type: %d", mDataType);
	}
}

void Texture::GenerateMipmaps() const
{
	glGenerateMipmap(GL_TEXTURE_2D);
}
