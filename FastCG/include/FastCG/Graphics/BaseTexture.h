#ifndef FASTCG_BASE_TEXTURE_H
#define FASTCG_BASE_TEXTURE_H

#include <FastCG/Graphics/GraphicsEnums.h>

#include <string>
#include <string.h>
#include <memory>
#include <cstdint>

namespace FastCG
{
	struct TextureArgs
	{
		std::string name;
		uint32_t width;
		uint32_t height;
		TextureType type{TextureType::TEXTURE_2D};
		TextureFormat format{TextureFormat::RGBA};
		BitsPerPixel bitsPerPixel{8, 8, 8, 8};
		TextureDataType dataType{TextureDataType::UNSIGNED_CHAR};
		TextureFilter filter{TextureFilter::LINEAR_FILTER};
		TextureWrapMode wrapMode{TextureWrapMode::CLAMP};
		bool generateMipmap{true};
		const void *pData{nullptr};
	};

	class BaseTexture
	{
	public:
		inline const std::string &GetName() const
		{
			return mName;
		}

		inline uint32_t GetHeight() const
		{
			return mHeight;
		}

		inline uint32_t GetWidth() const
		{
			return mWidth;
		}

		inline TextureType GetType() const
		{
			return mType;
		}

		inline TextureFormat GetFormat() const
		{
			return mFormat;
		}

		inline const BitsPerPixel &GetBitsPerPixel() const
		{
			return mBitsPerPixel;
		}

		inline TextureDataType GetDataType() const
		{
			return mDataType;
		}

		inline TextureFilter GetFilter() const
		{
			return mFilter;
		}

		inline TextureWrapMode GetWrapMode() const
		{
			return mWrapMode;
		}

		inline bool HasGeneratedMipmap() const
		{
			return mGenerateMipmap;
		}

		inline size_t GetDataSize() const
		{
			return (size_t)(mWidth * mHeight * ((mBitsPerPixel.r + mBitsPerPixel.g + mBitsPerPixel.b + mBitsPerPixel.a) / 8));
		}

		inline const uint8_t *GetData() const
		{
			return mpData.get();
		}

	protected:
		const std::string mName;
		uint32_t mWidth;
		uint32_t mHeight;
		TextureType mType;
		BitsPerPixel mBitsPerPixel;
		TextureFormat mFormat;
		TextureDataType mDataType;
		TextureFilter mFilter;
		TextureWrapMode mWrapMode;
		bool mGenerateMipmap;
		size_t mDataSize;
		std::unique_ptr<uint8_t[]> mpData;

		BaseTexture(const TextureArgs &rArgs) : mName(rArgs.name),
												mWidth(rArgs.width),
												mHeight(rArgs.height),
												mType(rArgs.type),
												mFormat(rArgs.format),
												mBitsPerPixel(rArgs.bitsPerPixel),
												mDataType(rArgs.dataType),
												mFilter(rArgs.filter),
												mWrapMode(rArgs.wrapMode),
												mGenerateMipmap(rArgs.generateMipmap)
		{
			auto dataSize = GetDataSize();
			if (dataSize > 0)
			{
				mpData = std::make_unique<uint8_t[]>(dataSize);
				if (rArgs.pData != nullptr)
				{
					memcpy((void *)mpData.get(), rArgs.pData, dataSize);
				}
			}
		}
		virtual ~BaseTexture() = default;
	};

}

#endif
