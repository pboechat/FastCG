#include <Texture.h>
#include <OpenGLExceptions.h>

Texture::Texture(unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned int filter, unsigned int wrap, void* pData) :
	mTextureId(0),
	mWidth(width), 
	mHeight(height), 
	mInternalFormat(internalFormat), 
	mFormat(format), 
	mType(type), 
	mFilter(filter), 
	mWrap(wrap), 
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrap);

	if (mType == GL_UNSIGNED_BYTE)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, mpData);
	glGenerateMipmap(GL_TEXTURE_2D);

	CHECK_FOR_OPENGL_ERRORS();
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
