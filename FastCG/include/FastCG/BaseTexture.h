#ifndef FASTCG_BASE_TEXTURE_H
#define FASTCG_BASE_TEXTURE_H

#include <string>
#include <cstdint>

namespace FastCG
{
	enum class TextureType : uint8_t
	{
		TEXTURE_2D

	};

	enum class TextureWrapMode : uint8_t
	{
		CLAMP = 0,
		REPEAT

	};

	enum class TextureFilter : uint8_t
	{
		POINT_FILTER = 0,
		LINEAR_FILTER

	};

	enum class TextureFormat : uint8_t
	{
		R = 0,
		RG,
		RGB,
		RGBA,
		BGR,
		BGRA,
		DEPTH_STENCIL

	};

	enum class TextureDataType : uint8_t
	{
		FLOAT = 0,
		UNSIGNED_CHAR,
		UNSIGNED_INT

	};

	struct BitsPerPixel
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

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
		bool generateMipmaps{true};
		void *pData{nullptr};
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

		inline const BitsPerPixel& GetBitsPerPixel() const
		{
			return mBitsPerPixel;
		}

		inline TextureDataType GetDataType() const
		{
			return mDataType;
		}

		inline TextureFilter GetFilterMode() const
		{
			return mFilter;
		}

		inline TextureWrapMode GetWrapMode() const
		{
			return mWrapMode;
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

		BaseTexture(const TextureArgs &rArgs) : mName(rArgs.name),
												mWidth(rArgs.width),
												mHeight(rArgs.height),
												mType(rArgs.type),
												mFormat(rArgs.format),
												mBitsPerPixel(rArgs.bitsPerPixel),
												mDataType(rArgs.dataType),
												mFilter(rArgs.filter),
												mWrapMode(rArgs.wrapMode)
		{
		}
		virtual ~BaseTexture() = default;
	};

}

#endif
