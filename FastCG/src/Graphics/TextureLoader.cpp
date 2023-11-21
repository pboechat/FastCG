#include <FastCG/Platform/File.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Core/Exception.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <memory>
#include <cstdint>

namespace FastCG
{
	Texture *TextureLoader::Load(const std::string &rFilePath, TextureSamplerSettings samplerSettings)
	{
		int width, height, components;
		auto *pPixels = stbi_load(rFilePath.c_str(), &width, &height, &components, 0);
		if (pPixels == nullptr)
		{
			return nullptr;
		}

		std::unique_ptr<uint8_t[]> transformedPixels;
		TextureFormat format;
		BitsPerChannel bitsPerChannel;
		switch (components)
		{
		case 1:
			format = FastCG::TextureFormat::R;
			bitsPerChannel = {8};
			break;
		case 2:
			format = FastCG::TextureFormat::RG;
			bitsPerChannel = {8, 8};
			break;
		case 3:
		{
			transformedPixels = std::make_unique<uint8_t[]>(width * height * 4 * sizeof(uint8_t));
			int i = 0, j = 0;
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					for (int c = 0; c < 3; ++c)
					{
						transformedPixels[i++] = pPixels[j++];
					}
					transformedPixels[i++] = 1;
				}
			}
			stbi_image_free(pPixels);
			pPixels = transformedPixels.get();
			components = 4;
		}
		case 4:
			format = FastCG::TextureFormat::RGBA;
			bitsPerChannel = {8, 8, 8, 8};
			break;
		default:
			FASTCG_THROW_EXCEPTION(Exception, "Couldn't get texture format and bits per channel (components=%d)", components);
			return nullptr;
		}

		auto *pTexture = GraphicsSystem::GetInstance()->CreateTexture({File::GetFileNameWithoutExtension(rFilePath),
																	   (uint32_t)width,
																	   (uint32_t)height,
																	   1,
																	   1,
																	   TextureType::TEXTURE_2D,
																	   TextureUsageFlagBit::SAMPLED,
																	   format,
																	   bitsPerChannel,
																	   TextureDataType::FLOAT,
																	   samplerSettings.filter,
																	   samplerSettings.wrapMode,
																	   pPixels});
		if (transformedPixels == nullptr)
		{
			stbi_image_free(pPixels);
		}

		return pTexture;
	}

}
