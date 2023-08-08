#ifndef FASTCG_BASE_TEXTURE_H
#define FASTCG_BASE_TEXTURE_H

#include <FastCG/Graphics/GraphicsUtils.h>

#include <string>
#include <memory>
#include <cstring>
#include <cstdint>

namespace FastCG
{
	class BaseTexture
	{
	public:
		struct Args
		{
			std::string name;
			uint32_t width;
			uint32_t height;
			TextureType type;
			TextureUsageFlags usage;
			TextureFormat format;
			BitsPerChannel bitsPerChannel;
			TextureDataType dataType;
			TextureFilter filter;
			TextureWrapMode wrapMode;
			bool generateMipmap;
			const uint8_t *pData;

			Args(const std::string &rName = "",
				 uint32_t width = 1,
				 uint32_t height = 1,
				 TextureType type = TextureType::TEXTURE_2D,
				 TextureUsageFlags usage = TextureUsageFlagBit::SAMPLED,
				 TextureFormat format = TextureFormat::RGBA,
				 BitsPerChannel bitsPerChannel = {8, 8, 8, 8},
				 TextureDataType dataType = TextureDataType::UNSIGNED_CHAR,
				 TextureFilter filter = TextureFilter::LINEAR_FILTER,
				 TextureWrapMode wrapMode = TextureWrapMode::CLAMP,
				 bool generateMipmap = true,
				 const uint8_t *pData = nullptr)
				: name(rName),
				  width(width),
				  height(height),
				  type(type),
				  usage(usage),
				  format(format),
				  bitsPerChannel(bitsPerChannel),
				  dataType(dataType),
				  filter(filter),
				  wrapMode(wrapMode),
				  generateMipmap(generateMipmap),
				  pData(pData)
			{
			}
		};

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

		inline TextureUsageFlags GetUsage() const
		{
			return mUsage;
		}

		inline TextureFormat GetFormat() const
		{
			return mFormat;
		}

		inline const BitsPerChannel &GetBitsPerChannel() const
		{
			return mBitsPerChannel;
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
			return (size_t)(mWidth * mHeight * ((mBitsPerChannel.r + mBitsPerChannel.g + mBitsPerChannel.b + mBitsPerChannel.a) >> 3));
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
		TextureUsageFlags mUsage;
		BitsPerChannel mBitsPerChannel;
		TextureFormat mFormat;
		TextureDataType mDataType;
		TextureFilter mFilter;
		TextureWrapMode mWrapMode;
		bool mGenerateMipmap;
		std::unique_ptr<uint8_t[]> mpData;

		BaseTexture(const Args &rArgs) : mName(rArgs.name),
										 mWidth(rArgs.width),
										 mHeight(rArgs.height),
										 mType(rArgs.type),
										 mUsage(rArgs.usage),
										 mFormat(rArgs.format),
										 mBitsPerChannel(rArgs.bitsPerChannel),
										 mDataType(rArgs.dataType),
										 mFilter(rArgs.filter),
										 mWrapMode(rArgs.wrapMode),
										 mGenerateMipmap(rArgs.generateMipmap)
		{
			if (rArgs.pData != nullptr)
			{
				auto dataSize = GetDataSize();
				mpData = std::make_unique<uint8_t[]>(dataSize);
				memcpy((void *)mpData.get(), rArgs.pData, dataSize);
			}
		}
		virtual ~BaseTexture() = default;
	};

}

#endif
