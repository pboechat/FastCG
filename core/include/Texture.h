#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <GL3/gl3w.h>

#include <string>

class Texture
{
public:
	Texture(unsigned int width, unsigned int height, unsigned int internalFormat = GL_RGB32F, unsigned int format = GL_RGB, unsigned int type = GL_FLOAT, unsigned int filter = GL_LINEAR, unsigned int wrap = GL_CLAMP_TO_EDGE, void* pData = 0);
	~Texture();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetHeight() const
	{
		return mHeight;
	}

	inline unsigned int GetWidth() const
	{
		return mWidth;
	}

	inline unsigned int GetInternalFormat() const
	{
		return mInternalFormat;
	}

	inline unsigned int GetFormat() const
	{
		return mFormat;
	}

	inline unsigned int GetType() const
	{
		return mType;
	}

	inline unsigned int GetFilter() const
	{
		return mFilter;
	}

	inline unsigned int GetWrap() const
	{
		return mWrap;
	}

private:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mInternalFormat;
	unsigned int mFormat;
	unsigned int mType;
	unsigned int mFilter;
	unsigned int mWrap;
	unsigned int mTextureId;
	void* mpData;

	void AllocateResources();
	void DeallocateResources();

};

#endif
