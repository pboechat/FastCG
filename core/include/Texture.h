#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <Pointer.h>

#include <string>

enum WrapMode
{
	WM_CLAMP, WM_REPEAT
};

enum FilterMode
{
	FM_POINT, FM_BILINEAR, FM_TRILINEAR
};

enum TextureFormat
{
	TF_RGB, TF_RGBA
};

enum DataType
{
	DT_FLOAT, DT_UNSIGNED_CHAR
};

class Texture
{
public:
	static Pointer<Texture> LoadPNG(const std::string& rFileName);

	Texture(unsigned int width, unsigned int height, TextureFormat format = TF_RGB, DataType dataType = DT_FLOAT, FilterMode filter = FM_BILINEAR, WrapMode wrapMode = WM_CLAMP, void* pData = 0);
	~Texture();

	void Bind() const;

	inline FilterMode GetFilter() const
	{
		return mFilter;
	}

	inline TextureFormat GetFormat() const
	{
		return mFormat;
	}

	inline DataType GetDataType() const
	{
		return mDataType;
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
	unsigned int GetFormatMapping() const;
	unsigned int GetDataTypeMapping() const;
	void GenerateMipmaps() const;

};

typedef Pointer<Texture> TexturePtr;

#endif
