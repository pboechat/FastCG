#include <FastCG/Texture.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
	uint32_t GetGLFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::TF_POINT_FILTER:
			return GL_NEAREST;
		case TextureFilter::TF_LINEAR_FILTER:
			return GL_LINEAR;
		default:
			THROW_EXCEPTION(Exception, "Unhandled filter: %d", filter);
			return 0;
		}
	}

	uint32_t GetGLWrapMode(TextureWrapMode wrapMode)
	{
		switch (wrapMode)
		{
		case TextureWrapMode::TW_CLAMP:
			return GL_CLAMP;
		case TextureWrapMode::TW_REPEAT:
			return GL_REPEAT;
		default:
			THROW_EXCEPTION(Exception, "Unhandled wrapping mode: %d", wrapMode);
			return 0;
		}
	}

	uint32_t GetGLFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::TF_R:
			return GL_RED;
		case TextureFormat::TF_RG:
			return GL_RG;
		case TextureFormat::TF_RGB:
			return GL_RGB;
		case TextureFormat::TF_RGBA:
			return GL_RGBA;
		case TextureFormat::TF_BGR:
			return GL_BGR;
		case TextureFormat::TF_BGRA:
			return GL_BGRA;
		default:
			THROW_EXCEPTION(Exception, "Unhandled internal format: %d", format);
			return 0;
		}
	}

	uint32_t GetGLDataType(TextureDataType dataType)
	{
		switch (dataType)
		{
		case TextureDataType::DT_FLOAT:
			return GL_FLOAT;
		case TextureDataType::DT_UNSIGNED_CHAR:
			return GL_UNSIGNED_BYTE;
		default:
			THROW_EXCEPTION(Exception, "Unhandled data type: %d", dataType);
			return 0;
		}
	}

	Texture::Texture(uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, TextureFilter filter, TextureWrapMode wrapMode, bool generateMipmaps, void* pData) :
		mWidth(width),
		mHeight(height),
		mFormat(format),
		mDataType(dataType),
		mFilter(filter),
		mWrapMode(wrapMode)
	{
		AllocateResources(pData, generateMipmaps);
	}

	Texture::~Texture()
	{
		DeallocateResources();
	}

	void Texture::AllocateResources(void* pData, bool generateMipmaps)
	{
		auto filter = GetGLFilter(mFilter);
		auto wrapMode = GetGLWrapMode(mWrapMode);
		auto format = GetGLFormat(mFormat);
		auto dataType = GetGLDataType(mDataType);

		glGenTextures(1, &mTextureId);
		glBindTexture(GL_TEXTURE_2D, mTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, dataType, pData);

		if (generateMipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		CHECK_FOR_OPENGL_ERRORS();
	}

	void Texture::DeallocateResources()
	{
		if (mTextureId != ~0u)
		{
			glDeleteTextures(1, &mTextureId);
		}
	}

	void Texture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, mTextureId);
		CHECK_FOR_OPENGL_ERRORS();
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		CHECK_FOR_OPENGL_ERRORS();
	}

}