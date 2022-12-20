#include <FastCG/TextureLoader.h>
#include <FastCG/File.h>
#include <FastCG/Exception.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <cstdint>
#include <cassert>

namespace
{
	bool GetTextureFormatAndBitsPerPixel(int components, FastCG::TextureFormat &format, FastCG::BitsPerPixel &bitsPerPixel)
	{
		if (components == 1)
		{
			format = FastCG::TextureFormat::R;
			bitsPerPixel = {8};
			return true;
		}
		else if (components == 2)
		{
			format = FastCG::TextureFormat::RG;
			bitsPerPixel = {8, 8};
			return true;
		}
		else if (components == 3)
		{
			format = FastCG::TextureFormat::RGB;
			bitsPerPixel = {8, 8, 8};
			return true;
		}
		else if (components == 4)
		{
			format = FastCG::TextureFormat::RGBA;
			bitsPerPixel = {8, 8, 8, 8};
			return true;
		}
		else
		{
			format = (FastCG::TextureFormat)0;
			bitsPerPixel = {};
			return false;
		}
	}

}

namespace FastCG
{
	Texture *TextureLoader::Load(const std::string &rFilePath)
	{
		int width, height, components;
		auto *pData = stbi_load(rFilePath.c_str(), &width, &height, &components, 0);
		if (pData == nullptr)
		{
			return nullptr;
		}
		TextureFormat format;
		BitsPerPixel bitsPerPixel;
		if (!GetTextureFormatAndBitsPerPixel(components, format, bitsPerPixel))
		{
			FASTCG_THROW_EXCEPTION(Exception, "Couldn't get texture format and bits per pixel (components = %d)", components);
		}
		auto *pTexture = RenderingSystem::GetInstance()->CreateTexture({File::GetFileNameWithoutExtension(rFilePath),
																		(uint32_t)width,
																		(uint32_t)height,
																		TextureType::TEXTURE_2D,
																		format,
																		bitsPerPixel,
																		TextureDataType::UNSIGNED_CHAR,
																		TextureFilter::LINEAR_FILTER,
																		TextureWrapMode::REPEAT,
																		true,
																		(void *)pData});
		stbi_image_free(pData);
		return pTexture;
	}

}