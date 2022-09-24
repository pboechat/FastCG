#ifndef FASTCG_TEXTURE_H_
#define FASTCG_TEXTURE_H_

#include <GL/glew.h>
#include <GL/gl.h>

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

	class Texture
	{
	public:
		Texture(const std::string& rName,
			uint32_t width,
			uint32_t height,
			TextureFormat format = TextureFormat::TF_RGB,
			TextureDataType dataType = TextureDataType::DT_UNSIGNED_CHAR,
			TextureFilter filter = TextureFilter::TF_LINEAR_FILTER,
			TextureWrapMode wrapMode = TextureWrapMode::TW_CLAMP,
			bool generateMipmaps = true,
			void* pData = 0);
		~Texture();

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetHeight() const
		{
			return mHeight;
		}

		inline uint32_t GetWidth() const
		{
			return mWidth;
		}

		inline TextureFormat GetInternalFormat() const
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

	private:
		uint32_t mWidth;
		uint32_t mHeight;
		TextureFormat mFormat;
		TextureDataType mDataType;
		TextureFilter mFilter;
		TextureWrapMode mWrapMode;
		GLuint mTextureId{ ~0u };

		void AllocateResources(const std::string& rName, void* pData, bool generateMipmaps);
		void DeallocateResources();

	};

}

#endif
