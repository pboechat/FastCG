#include <FastCG/TextureImporter.h>
#include <FastCG/File.h>
#include <FastCG/AssetSystem.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <cstdint>
#include <cassert>

namespace
{
	FastCG::TextureFormat GetTextureFormat(int components)
	{
		if (components == 1)
		{
			return FastCG::TextureFormat::TF_R;
		}
		else if (components == 2)
		{
			return FastCG::TextureFormat::TF_RG;
		}
		else if (components == 3)
		{
			return FastCG::TextureFormat::TF_RGB;
		}
		else if (components == 4)
		{
			return FastCG::TextureFormat::TF_RGBA;
		}
		else
		{
			assert(false);
			return (FastCG::TextureFormat)0;
		}
	}

}

namespace FastCG
{
	Texture *TextureImporter::Import(const std::string &rFilePath)
	{
		int width, height, components;
		auto absFfilePath = AssetSystem::GetInstance()->Resolve(rFilePath);
		auto *pData = stbi_load(absFfilePath.c_str(), &width, &height, &components, 0);
		if (pData == nullptr)
		{
			return nullptr;
		}
		auto *pTexture = RenderingSystem::GetInstance()->CreateTexture({File::GetFileNameWithoutExtension(rFilePath),
																		(uint32_t)width,
																		(uint32_t)height,
																		GetTextureFormat(components),
																		TextureDataType::DT_UNSIGNED_CHAR,
																		TextureFilter::TF_LINEAR_FILTER,
																		TextureWrapMode::TW_REPEAT,
																		true,
																		(void *)pData});
		stbi_image_free(pData);
		return pTexture;
	}

}