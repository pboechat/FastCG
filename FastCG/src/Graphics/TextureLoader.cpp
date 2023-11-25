#include <FastCG/Platform/File.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Exception.h>

#include <stb_image.h>
#include <ktx.h>

#include <vector>
#include <memory>
#include <cstdint>

// FIXME:

#define KTX2_VK_FORMAT_R32_SFLOAT 100
#define KTX2_VK_FORMAT_R16_SFLOAT 76
#define KTX2_VK_FORMAT_R16_UNORM 70
#define KTX2_VK_FORMAT_R8_UNORM 9
#define KTX2_VK_FORMAT_R32G32_SFLOAT 103
#define KTX2_VK_FORMAT_R16G16_SFLOAT 83
#define KTX2_VK_FORMAT_R16G16_UNORM 77
#define KTX2_VK_FORMAT_R8G8_UNORM 16
#define KTX2_VK_FORMAT_R32G32B32_SFLOAT 106
#define KTX2_VK_FORMAT_R16G16B16_UNORM 84
#define KTX2_VK_FORMAT_R16G16B16_SFLOAT 90
#define KTX2_VK_FORMAT_R8G8B8_UNORM 147
#define KTX2_VK_FORMAT_B8G8R8_UNORM 30
#define KTX2_VK_FORMAT_B10G11R11_UFLOAT_PACK32 122
#define KTX2_VK_FORMAT_R32G32B32A32_SFLOAT 109
#define KTX2_VK_FORMAT_R16G16B16A16_UNORM 91
#define KTX2_VK_FORMAT_A2R10G10B10_UNORM_PACK32 58
#define KTX2_VK_FORMAT_R8G8B8A8_UNORM 37
#define KTX2_VK_FORMAT_B8G8R8A8_UNORM 44
#define KTX2_VK_FORMAT_D24_UNORM_S8_UINT 129
#define KTX2_VK_FORMAT_D32_SFLOAT 126
#define KTX2_VK_FORMAT_X8_D24_UNORM_PACK32 125
#define KTX2_VK_FORMAT_D16_UNORM 128
#define KTX2_VK_FORMAT_BC1_RGB_UNORM_BLOCK 131
#define KTX2_VK_FORMAT_BC1_RGBA_UNORM_BLOCK 133
#define KTX2_VK_FORMAT_BC2_UNORM_BLOCK 135
#define KTX2_VK_FORMAT_BC3_UNORM_BLOCK 137
#define KTX2_VK_FORMAT_BC4_UNORM_BLOCK 139
#define KTX2_VK_FORMAT_BC4_SNORM_BLOCK 140
#define KTX2_VK_FORMAT_BC5_UNORM_BLOCK 141
#define KTX2_VK_FORMAT_BC5_SNORM_BLOCK 142
#define KTX2_VK_FORMAT_BC6H_UFLOAT_BLOCK 143
#define KTX2_VK_FORMAT_BC6H_SFLOAT_BLOCK 144
#define KTX2_VK_FORMAT_BC7_UNORM_BLOCK 145
#define KTX2_VK_FORMAT_ASTC_4x4_UNORM_BLOCK 157
#define KTX2_VK_FORMAT_ASTC_5x4_UNORM_BLOCK 159
#define KTX2_VK_FORMAT_ASTC_5x5_UNORM_BLOCK 161
#define KTX2_VK_FORMAT_ASTC_6x5_UNORM_BLOCK 163
#define KTX2_VK_FORMAT_ASTC_6x6_UNORM_BLOCK 165
#define KTX2_VK_FORMAT_ASTC_8x5_UNORM_BLOCK 167
#define KTX2_VK_FORMAT_ASTC_8x6_UNORM_BLOCK 169
#define KTX2_VK_FORMAT_ASTC_8x8_UNORM_BLOCK 171
#define KTX2_VK_FORMAT_ASTC_10x5_UNORM_BLOCK 173
#define KTX2_VK_FORMAT_ASTC_10x6_UNORM_BLOCK 175
#define KTX2_VK_FORMAT_ASTC_10x8_UNORM_BLOCK 177
#define KTX2_VK_FORMAT_ASTC_10x10_UNORM_BLOCK 179
#define KTX2_VK_FORMAT_ASTC_12x10_UNORM_BLOCK 181
#define KTX2_VK_FORMAT_ASTC_12x12_UNORM_BLOCK 183

namespace
{
	FastCG::TextureFormat GetTextureFormat(ktxTexture2 *pKtxTexture2)
	{
#ifdef CASE_RETURN
#undef CASE_RETURN
#endif
#define CASE_RETURN(format)       \
	case KTX2_VK_FORMAT_##format: \
		return FastCG::TextureFormat::format

		switch (pKtxTexture2->vkFormat)
		{
			CASE_RETURN(R32_SFLOAT);
			CASE_RETURN(R16_SFLOAT);
			CASE_RETURN(R16_UNORM);
			CASE_RETURN(R8_UNORM);
			CASE_RETURN(R32G32_SFLOAT);
			CASE_RETURN(R16G16_SFLOAT);
			CASE_RETURN(R16G16_UNORM);
			CASE_RETURN(R8G8_UNORM);
			CASE_RETURN(R32G32B32_SFLOAT);
			CASE_RETURN(R16G16B16_UNORM);
			CASE_RETURN(R16G16B16_SFLOAT);
			CASE_RETURN(R8G8B8_UNORM);
			CASE_RETURN(B8G8R8_UNORM);
			CASE_RETURN(B10G11R11_UFLOAT_PACK32);
			CASE_RETURN(R32G32B32A32_SFLOAT);
			CASE_RETURN(R16G16B16A16_UNORM);
			CASE_RETURN(A2R10G10B10_UNORM_PACK32);
			CASE_RETURN(R8G8B8A8_UNORM);
			CASE_RETURN(B8G8R8A8_UNORM);
			CASE_RETURN(D24_UNORM_S8_UINT);
			CASE_RETURN(D32_SFLOAT);
			CASE_RETURN(X8_D24_UNORM_PACK32);
			CASE_RETURN(D16_UNORM);
			CASE_RETURN(BC1_RGB_UNORM_BLOCK);
			CASE_RETURN(BC1_RGBA_UNORM_BLOCK);
			CASE_RETURN(BC2_UNORM_BLOCK);
			CASE_RETURN(BC3_UNORM_BLOCK);
			CASE_RETURN(BC4_UNORM_BLOCK);
			CASE_RETURN(BC4_SNORM_BLOCK);
			CASE_RETURN(BC5_UNORM_BLOCK);
			CASE_RETURN(BC5_SNORM_BLOCK);
			CASE_RETURN(BC6H_UFLOAT_BLOCK);
			CASE_RETURN(BC6H_SFLOAT_BLOCK);
			CASE_RETURN(BC7_UNORM_BLOCK);
			CASE_RETURN(ASTC_4x4_UNORM_BLOCK);
			CASE_RETURN(ASTC_5x4_UNORM_BLOCK);
			CASE_RETURN(ASTC_5x5_UNORM_BLOCK);
			CASE_RETURN(ASTC_6x5_UNORM_BLOCK);
			CASE_RETURN(ASTC_6x6_UNORM_BLOCK);
			CASE_RETURN(ASTC_8x5_UNORM_BLOCK);
			CASE_RETURN(ASTC_8x6_UNORM_BLOCK);
			CASE_RETURN(ASTC_8x8_UNORM_BLOCK);
			CASE_RETURN(ASTC_10x5_UNORM_BLOCK);
			CASE_RETURN(ASTC_10x6_UNORM_BLOCK);
			CASE_RETURN(ASTC_10x8_UNORM_BLOCK);
			CASE_RETURN(ASTC_10x10_UNORM_BLOCK);
			CASE_RETURN(ASTC_12x10_UNORM_BLOCK);
			CASE_RETURN(ASTC_12x12_UNORM_BLOCK);
		}
		FASTCG_THROW_EXCEPTION(FastCG::Exception, "Can't get texture format (vkFormat: %d)", (int)pKtxTexture2->vkFormat);
		return (FastCG::TextureFormat)0;

#undef CASE_RETURN
	}

	FastCG::Texture *LoadKtxTexture(const std::string &rFilePath, FastCG::TextureSamplerSettings samplerSettings)
	{
		ktxTexture *pKtxTexture;

		if (ktxTexture_CreateFromNamedFile(rFilePath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &pKtxTexture) != KTX_SUCCESS)
		{
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Failed to read KTX texture (texture: %s)", rFilePath.c_str());
			return nullptr;
		}

		if (pKtxTexture->classId != ktxTexture2_c)
		{
			ktxTexture_Destroy(pKtxTexture);
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Only KTX2 are supported at the moment (texture: %s)", rFilePath.c_str()); // TODO:
			return nullptr;
		}
		auto *pKtxTexture2 = reinterpret_cast<ktxTexture2 *>(pKtxTexture);

		auto format = GetTextureFormat(pKtxTexture2);
		auto width = (uint32_t)pKtxTexture->baseWidth;
		auto height = (uint32_t)pKtxTexture->baseHeight;
		auto depth = (uint32_t)pKtxTexture->baseDepth;
		auto layers = (uint32_t)pKtxTexture->numLayers; // array
		auto mipCount = (uint32_t)pKtxTexture->numLevels;

		FastCG::TextureType type;
		uint32_t depthOrSlices;

		std::unique_ptr<uint8_t[]> pixels;
		if ((pKtxTexture->numDimensions == 1 || pKtxTexture->numDimensions == 2) && !pKtxTexture->isArray && !pKtxTexture->isCubemap)
		{
			assert(layers == 1);
			type = pKtxTexture->numDimensions == 1 ? FastCG::TextureType::TEXTURE_1D : FastCG::TextureType::TEXTURE_2D;
			// force slices to 1
			// TODO: support 3D/array/cubemap mips
			depthOrSlices = 1;

			struct MipStats
			{
				uint32_t width;
				uint32_t height;
				size_t size;
			};
			size_t totalSize = 0;
			std::vector<MipStats> mipStats(mipCount);
			for (uint32_t mip = 0; mip < mipCount; ++mip)
			{
				auto mipWidth = width >> mip;
				auto mipHeight = height >> mip;
				auto mipSize = GetTextureDataSize(format, mipWidth, mipHeight, 1);
				mipStats[mip] = {mipWidth, mipHeight, mipSize};
				totalSize += mipSize;
			}
			pixels = std::make_unique<uint8_t[]>(totalSize);
			auto *pPixels = &pixels[0];
			for (uint32_t mip = 0; mip < mipCount; ++mip)
			{
				ktx_size_t mipOffset;
				ktxTexture_GetImageOffset(pKtxTexture, (ktx_uint32_t)mip, 0, 0, &mipOffset);
				memcpy(pPixels, pKtxTexture->pData + mipOffset, mipStats[mip].size);
				pPixels += mipStats[mip].size;
			}
		}
		else
		{
			if (pKtxTexture->numDimensions == 3)
			{
				type = FastCG::TextureType::TEXTURE_3D;
				// force mipCount to 1
				// TODO: support 3D mips
				mipCount = 1;
				// TODO: support 3D arrays
				depthOrSlices = depth;

				size_t totalSize = GetTextureDataSize(format, width, height, depth);
				pixels = std::make_unique<uint8_t[]>(totalSize);
				ktx_size_t offset;
				ktxTexture_GetImageOffset(pKtxTexture, 0, 0, 0, &offset);
				memcpy(&pixels[0], pKtxTexture->pData + offset, totalSize);
			}
			else if (pKtxTexture->isCubemap)
			{
				type = FastCG::TextureType::TEXTURE_CUBE_MAP;
				// force mipCount to 1
				// TODO: support cubemap mips
				mipCount = 1;
				assert(depth == 1);
				assert(pKtxTexture->numFaces == 6); // FIXME: invariant checking
				depthOrSlices = layers = 6;

				auto sliceSize = GetTextureDataSize(format, width, height, 1);
				pixels = std::make_unique<uint8_t[]>(sliceSize * layers);
				auto *pPixels = &pixels[0];
				for (uint32_t slice = 0; slice < layers; ++slice)
				{
					ktx_size_t sliceOffset;
					ktxTexture_GetImageOffset(pKtxTexture, 0, 0, (ktx_uint32_t)slice, &sliceOffset);
					memcpy(pPixels, pKtxTexture->pData + sliceOffset, sliceSize);
					pPixels += sliceSize;
				}
			}
			else
			{
				assert(pKtxTexture->isArray);
				assert(pKtxTexture->numDimensions == 2);
				type = FastCG::TextureType::TEXTURE_2D_ARRAY;
				// force mipCount to 1
				// TODO: support 3D mips
				mipCount = 1;
				// force depth to 1
				// TODO: support 3D arrays
				depth = 1;
				depthOrSlices = layers;

				auto sliceSize = GetTextureDataSize(format, width, height, 1);
				pixels = std::make_unique<uint8_t[]>(sliceSize * layers);
				auto *pPixels = &pixels[0];
				for (uint32_t slice = 0; slice < layers; ++slice)
				{
					ktx_size_t sliceOffset;
					ktxTexture_GetImageOffset(pKtxTexture, 0, slice, 0, &sliceOffset);
					memcpy(pPixels, pKtxTexture->pData + sliceOffset, sliceSize);
					pPixels += sliceSize;
				}
			}
		}

		ktxTexture_Destroy(pKtxTexture);

		return FastCG::GraphicsSystem::GetInstance()->CreateTexture({rFilePath,
																	 width,
																	 height,
																	 depthOrSlices,
																	 (uint8_t)mipCount,
																	 type,
																	 FastCG::TextureUsageFlagBit::SAMPLED,
																	 format,
																	 samplerSettings.filter,
																	 samplerSettings.wrapMode,
																	 &pixels[0]});
	}

	FastCG::Texture *LoadPlainTexture(const std::string &rFilePath, FastCG::TextureSamplerSettings samplerSettings)
	{
		int width, height, componentCount;
		auto *pPixels = stbi_load(rFilePath.c_str(), &width, &height, &componentCount, 0);
		if (pPixels == nullptr)
		{
			return nullptr;
		}

		std::unique_ptr<uint8_t[]> transformedPixels;
		FastCG::TextureFormat format;
		switch (componentCount)
		{
		case 1:
			format = FastCG::TextureFormat::R8_UNORM;
			break;
		case 2:
			format = FastCG::TextureFormat::R8G8_UNORM;
			break;
		case 3:
		{
			// make it RGBA and add alpha 1
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
			componentCount = 4;
		}
		case 4:
			format = FastCG::TextureFormat::R8G8B8A8_UNORM;
			break;
		default:
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Couldn't get format and bits per channel (texture: %s, componentCount: %d)", rFilePath.c_str(), componentCount);
			return nullptr;
		}

		auto *pTexture = FastCG::GraphicsSystem::GetInstance()->CreateTexture({rFilePath,
																			   (uint32_t)width,
																			   (uint32_t)height,
																			   1,
																			   1,
																			   FastCG::TextureType::TEXTURE_2D,
																			   FastCG::TextureUsageFlagBit::SAMPLED,
																			   format,
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

namespace FastCG
{
	Texture *TextureLoader::Load(const std::string &rFilePath, TextureSamplerSettings samplerSettings)
	{
		std::string fileName, extension;
		File::SplitExt(rFilePath, fileName, extension);
		if (extension == ".ktx")
		{
			return LoadKtxTexture(rFilePath, samplerSettings);
		}
		else
		{
			return LoadPlainTexture(rFilePath, samplerSettings);
		}
	}

}
