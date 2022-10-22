#ifndef FASTCG_BASE_TEXTURE_H
#define FASTCG_BASE_TEXTURE_H

#include <string>
#include <cstdint>

namespace FastCG
{
	enum class TextureWrapMode : uint8_t
	{
		TW_CLAMP = 0,
		TW_REPEAT

	};

	enum class TextureFilter : uint8_t
	{
		TF_POINT_FILTER = 0,
		TF_LINEAR_FILTER

	};

	enum class TextureFormat : uint8_t
	{
		TF_R = 0,
		TF_RG,
		TF_RGB,
		TF_RGBA,
		TF_BGR,
		TF_BGRA

	};

	enum class TextureDataType : uint8_t
	{
		DT_FLOAT = 0,
		DT_UNSIGNED_CHAR

	};

	struct TextureArgs
	{
		std::string name;
		uint32_t width;
		uint32_t height;
		TextureFormat format{TextureFormat::TF_RGB};
		TextureDataType dataType{TextureDataType::DT_UNSIGNED_CHAR};
		TextureFilter filter{TextureFilter::TF_LINEAR_FILTER};
		TextureWrapMode wrapMode{TextureWrapMode::TW_CLAMP};
		bool generateMipmaps{true};
		void *pData{nullptr};
	};

	class BaseTexture
	{
	public:
		inline const std::string &GetName() const
		{
			return mArgs.name;
		}

		inline uint32_t GetHeight() const
		{
			return mArgs.height;
		}

		inline uint32_t GetWidth() const
		{
			return mArgs.width;
		}

		inline TextureFormat GetFormat() const
		{
			return mArgs.format;
		}

		inline TextureDataType GetDataType() const
		{
			return mArgs.dataType;
		}

		inline TextureFilter GetFilterMode() const
		{
			return mArgs.filter;
		}

		inline TextureWrapMode GetWrapMode() const
		{
			return mArgs.wrapMode;
		}

	protected:
		const TextureArgs mArgs;

		BaseTexture(const TextureArgs &rArgs) : mArgs(rArgs) {}
	};

}

#endif
