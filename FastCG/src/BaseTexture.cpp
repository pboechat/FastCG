#include <FastCG/BaseTexture.h>

namespace FastCG
{
	void BaseTexture::Initialize(const std::string &rName,
							 uint32_t width,
							 uint32_t height,
							 TextureFormat format,
							 TextureDataType dataType,
							 TextureFilter filter,
							 TextureWrapMode wrapMode,
							 bool generateMipmaps,
							 void *pData)
	{
		mName = rName;
		mWidth = width;
		mHeight = height;
		mFormat = format;
		mDataType = dataType;
		mFilter = filter;
		mWrapMode = wrapMode;
		OnInitialize(generateMipmaps, pData);
	}

}