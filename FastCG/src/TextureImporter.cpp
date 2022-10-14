#include <FastCG/TextureImporter.h>
#include <FastCG/Exception.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <cstdint>
#include <cassert>

namespace FastCG
{
	static std::string gBasePath;
	static std::vector<std::shared_ptr<Texture>> gManagedTextures;

	TextureFormat GetTextureFormat(int components)
	{
		if (components == 1)
		{
			return TextureFormat::TF_R;
		}
		else if (components == 2)
		{
			return TextureFormat::TF_RG;
		}
		else if (components == 3)
		{
			return TextureFormat::TF_RGB;
		}
		else if (components == 4)
		{
			return TextureFormat::TF_RGBA;
		}
		else
		{
			assert(false);
			return (TextureFormat)0;
		}
	}

	void TextureImporter::SetBasePath(const std::string &basePath)
	{
		gBasePath = basePath;
	}

	std::shared_ptr<Texture> TextureImporter::Import(const std::string &rFilePath)
	{
		int width, height, components;
		auto *pData = stbi_load((gBasePath + '/' + rFilePath).c_str(), &width, &height, &components, 0);
		if (pData == nullptr)
		{
			return nullptr;
		}
		auto pTexture = std::make_shared<Texture>();
		pTexture->Initialize(rFilePath,
							 (uint32_t)width,
							 (uint32_t)height,
							 GetTextureFormat(components),
							 TextureDataType::DT_UNSIGNED_CHAR,
							 TextureFilter::TF_LINEAR_FILTER,
							 TextureWrapMode::TW_REPEAT,
							 true,
							 (void *)pData);
		stbi_image_free(pData);
		gManagedTextures.emplace_back(pTexture);
		return pTexture;
	}

}