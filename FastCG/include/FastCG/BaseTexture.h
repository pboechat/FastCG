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

	class BaseTexture
	{
	public:
		BaseTexture() = default;
		virtual ~BaseTexture() = default;

		void Initialize(const std::string &rName,
						uint32_t width,
						uint32_t height,
						TextureFormat format = TextureFormat::TF_RGB,
						TextureDataType dataType = TextureDataType::DT_UNSIGNED_CHAR,
						TextureFilter filter = TextureFilter::TF_LINEAR_FILTER,
						TextureWrapMode wrapMode = TextureWrapMode::TW_CLAMP,
						bool generateMipmaps = true,
						void *pData = 0);
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

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

		inline TextureFormat GetFormat() const
		{
			return mFormat;
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
		virtual void OnInitialize(bool generateMipmaps, void *pData) = 0;

	private:
		std::string mName;
		uint32_t mWidth;
		uint32_t mHeight;
		TextureFormat mFormat;
		TextureDataType mDataType;
		TextureFilter mFilter;
		TextureWrapMode mWrapMode;
	};

}

#endif
