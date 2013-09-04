#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <string>

enum WrapMode
{
	WM_CLAMP, 
	WM_REPEAT
};

enum FilterMode
{
	FM_POINT_FILTER, 
	FM_LINEAR_FILTER, 
	FM_MIPMAPS
};

enum InternalFormat
{
	IF_RGB, 
	IF_RGBA,
	IF_BGR,
	IF_BGRA,
	IF_LUMINANCE,
	IF_LUMINANCE_ALPHA
};

enum DataType
{
	DF_FLOAT, 
	DF_UNSIGNED_CHAR
};

class Texture
{
public:
	Texture(unsigned int width, unsigned int height, unsigned int bytesPerPixel = 3, InternalFormat internalFormat = IF_RGB, DataType dataType = DF_FLOAT, FilterMode filter = FM_LINEAR_FILTER, WrapMode wrapMode = WM_CLAMP, void* pData = 0);
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

	inline unsigned int GetBytesPerPixel() const
	{
		return mBytesPerPixel;
	}

	inline InternalFormat GetInternalFormat() const
	{
		return mInternalFormat;
	}

	inline DataType GetDataType() const
	{
		return mDataType;
	}

	inline FilterMode GetFilterMode() const
	{
		return mFilter;
	}

	inline WrapMode GetWrapMode() const
	{
		return mWrapMode;
	}

private:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mBytesPerPixel;
	InternalFormat mInternalFormat;
	DataType mDataType;
	FilterMode mFilter;
	WrapMode mWrapMode;
	unsigned int mTextureId;
	void* mpData;

	void AllocateResources();
	void DeallocateResources();
	void SetUpFilter() const;
	void SetUpWrapping() const;
	unsigned int GetInternalFormatMapping() const;
	unsigned int GetDataTypeMapping() const;
	void GenerateMipmaps() const;

};

#endif
