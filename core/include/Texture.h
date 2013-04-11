#ifndef TEXTURE_H_
#define TEXTURE_H_

enum WrapMode
{
	WM_CLAMP,
	WM_REPEAT
};

enum FilterMode
{
	FM_POINT,
	FM_BILINEAR,
	FM_TRILINEAR
};

enum TextureFormat
{
	TF_RGB,
	TF_RGBA
};

class Texture
{
public:
	Texture(unsigned int width, unsigned int height, TextureFormat format, FilterMode filter, WrapMode wrapMode);
	Texture(unsigned int width, unsigned int height, TextureFormat format, FilterMode filter, WrapMode wrapMode, float* pData);
	virtual ~Texture();

	inline FilterMode GetFilter() const
	{
		return mFilter;
	}

	inline TextureFormat GetFormat() const
	{
		return mFormat;
	}

	inline unsigned int GetHeight() const
	{
		return mHeight;
	}

	inline unsigned int GetWidth() const
	{
		return mWidth;
	}

	inline WrapMode GetWrapMode() const
	{
		return mWrapMode;
	}

private:
	unsigned int mWidth;
	unsigned int mHeight;
	TextureFormat mFormat;
	FilterMode mFilter;
	WrapMode mWrapMode;
	unsigned int mTextureId;
	float* mpData;

	void AllocateResources();
	void DeallocateResources();

	void SetUpFilter() const;
	void SetUpWrapping() const;
	unsigned int GetGLInternalFormatMapping() const;
	unsigned int GetGLFormatMapping() const;
	void GenerateMipmaps() const;

};

#endif
