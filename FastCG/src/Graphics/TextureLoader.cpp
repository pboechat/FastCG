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
#define KTXT_VK_FORMAT_R8_UNORM 9
#define KTXT_VK_FORMAT_R8G8_UNORM 16
#define KTXT_VK_FORMAT_R8G8B8_UNORM 23
#define KTXT_VK_FORMAT_B8G8R8_UNORM 30
#define KTXT_VK_FORMAT_R8G8B8A8_UNORM 37
#define KTXT_VK_FORMAT_B8G8R8A8_UNORM 44
#define KTXT_VK_FORMAT_A2R10G10B10_UNORM_PACK32 58
#define KTXT_VK_FORMAT_R16_UNORM 70
#define KTXT_VK_FORMAT_R16_UINT 74
#define KTXT_VK_FORMAT_R16G16_UINT 81
#define KTXT_VK_FORMAT_R16G16_SFLOAT 83
#define KTXT_VK_FORMAT_R16G16B16_UNORM 84
#define KTXT_VK_FORMAT_R16G16B16A16_UNORM 91
#define KTXT_VK_FORMAT_R32_UINT 98
#define KTXT_VK_FORMAT_R32_SFLOAT 100
#define KTXT_VK_FORMAT_R32G32_UINT 101
#define KTXT_VK_FORMAT_R32G32_SFLOAT 103
#define KTXT_VK_FORMAT_R32G32B32_UINT 104
#define KTXT_VK_FORMAT_R32G32B32_SFLOAT 106
#define KTXT_VK_FORMAT_R32G32B32A32_UINT 107
#define KTXT_VK_FORMAT_R32G32B32A32_SFLOAT 109
#define KTXT_VK_FORMAT_B10G11R11_UFLOAT_PACK32 122
#define KTXT_VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 123
#define KTXT_VK_FORMAT_D16_UNORM 124
#define KTXT_VK_FORMAT_X8_D24_UNORM_PACK32 125
#define KTXT_VK_FORMAT_D32_SFLOAT 126
#define KTXT_VK_FORMAT_D24_UNORM_S8_UINT 129

namespace
{
	bool GetTextureFormatDataTypeAndBitsPerChannelFromKtxTexture2(ktxTexture2 *pKtxTexture2, FastCG::TextureFormat &rFormat, FastCG::TextureDataType &rDataType, FastCG::BitsPerChannel &rBitsPerChannel)
	{
#ifdef CASE
#undef CASE
#endif
#define CASE(vkFormat, format, bitsPerChannel, dataType) \
	case KTXT_##vkFormat:                                \
	{                                                    \
		rFormat = format;                                \
		rDataType = dataType;                            \
		rBitsPerChannel = bitsPerChannel;                \
		return true;                                     \
	}

		switch (pKtxTexture2->vkFormat)
		{
			CASE(VK_FORMAT_R32_SFLOAT, FastCG::TextureFormat::R, FastCG::BitsPerChannel{32}, FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_R32_UINT, FastCG::TextureFormat::R, FastCG::BitsPerChannel{32}, FastCG::TextureDataType::UNSIGNED_INT)
			CASE(VK_FORMAT_R16_UNORM, FastCG::TextureFormat::R, FastCG::BitsPerChannel{16}, FastCG::TextureDataType::UNSIGNED_SHORT)
			CASE(VK_FORMAT_R16_UINT, FastCG::TextureFormat::R, FastCG::BitsPerChannel{16}, FastCG::TextureDataType::UNSIGNED_SHORT)
			CASE(VK_FORMAT_R8_UNORM, FastCG::TextureFormat::R, FastCG::BitsPerChannel{8}, FastCG::TextureDataType::UNSIGNED_CHAR)
			CASE(VK_FORMAT_R32G32_SFLOAT, FastCG::TextureFormat::RG, (FastCG::BitsPerChannel{32, 32}), FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_R32G32_UINT, FastCG::TextureFormat::RG, (FastCG::BitsPerChannel{32, 32}), FastCG::TextureDataType::UNSIGNED_INT)
			CASE(VK_FORMAT_R16G16_SFLOAT, FastCG::TextureFormat::RG, (FastCG::BitsPerChannel{16, 16}), FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_R16G16_UINT, FastCG::TextureFormat::RG, (FastCG::BitsPerChannel{16, 16}), FastCG::TextureDataType::UNSIGNED_SHORT)
			CASE(VK_FORMAT_R8G8_UNORM, FastCG::TextureFormat::RG, (FastCG::BitsPerChannel{8, 8}), FastCG::TextureDataType::UNSIGNED_CHAR)
			CASE(VK_FORMAT_R32G32B32_SFLOAT, FastCG::TextureFormat::RGB, (FastCG::BitsPerChannel{32, 32, 32}), FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_R32G32B32_UINT, FastCG::TextureFormat::RGB, (FastCG::BitsPerChannel{32, 32, 32}), FastCG::TextureDataType::UNSIGNED_INT)
			CASE(VK_FORMAT_R16G16B16_UNORM, FastCG::TextureFormat::RGB, (FastCG::BitsPerChannel{16, 16, 16}), FastCG::TextureDataType::UNSIGNED_SHORT)
			CASE(VK_FORMAT_R8G8B8_UNORM, FastCG::TextureFormat::RGB, (FastCG::BitsPerChannel{8, 8, 8}), FastCG::TextureDataType::UNSIGNED_CHAR)
			CASE(VK_FORMAT_B8G8R8_UNORM, FastCG::TextureFormat::BGR, (FastCG::BitsPerChannel{8, 8, 8}), FastCG::TextureDataType::UNSIGNED_CHAR)
			CASE(VK_FORMAT_B10G11R11_UFLOAT_PACK32, FastCG::TextureFormat::RGB, (FastCG::BitsPerChannel{11, 11, 10}), FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_R32G32B32A32_SFLOAT, FastCG::TextureFormat::RGBA, (FastCG::BitsPerChannel{32, 32, 32, 32}), FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_R32G32B32A32_UINT, FastCG::TextureFormat::RGBA, (FastCG::BitsPerChannel{32, 32, 32, 32}), FastCG::TextureDataType::UNSIGNED_INT)
			CASE(VK_FORMAT_R16G16B16A16_UNORM, FastCG::TextureFormat::RGBA, (FastCG::BitsPerChannel{16, 16, 16, 16}), FastCG::TextureDataType::UNSIGNED_SHORT)
			CASE(VK_FORMAT_A2R10G10B10_UNORM_PACK32, FastCG::TextureFormat::RGBA, (FastCG::BitsPerChannel{10, 10, 10, 2}), FastCG::TextureDataType::UNSIGNED_INT)
			CASE(VK_FORMAT_R8G8B8A8_UNORM, FastCG::TextureFormat::RGBA, (FastCG::BitsPerChannel{8, 8, 8, 8}), FastCG::TextureDataType::UNSIGNED_CHAR)
			CASE(VK_FORMAT_B8G8R8A8_UNORM, FastCG::TextureFormat::BGRA, (FastCG::BitsPerChannel{8, 8, 8, 8}), FastCG::TextureDataType::UNSIGNED_CHAR)
			CASE(VK_FORMAT_D24_UNORM_S8_UINT, FastCG::TextureFormat::DEPTH_STENCIL, (FastCG::BitsPerChannel{24, 8}), FastCG::TextureDataType::FLOAT)
			CASE(VK_FORMAT_D16_UNORM, FastCG::TextureFormat::DEPTH, (FastCG::BitsPerChannel{16}), FastCG::TextureDataType::UNSIGNED_SHORT)
			CASE(VK_FORMAT_X8_D24_UNORM_PACK32, FastCG::TextureFormat::DEPTH, (FastCG::BitsPerChannel{24, 8}), FastCG::TextureDataType::UNSIGNED_INT)
			CASE(VK_FORMAT_D32_SFLOAT, FastCG::TextureFormat::DEPTH, (FastCG::BitsPerChannel{32}), FastCG::TextureDataType::FLOAT)
		}

		return false;

#undef CASE
	}

	FastCG::Texture *LoadKtxTexture(const std::string &rFilePath, FastCG::TextureSamplerSettings samplerSettings)
	{
		ktxTexture *pKtxTexture;

		if (ktxTexture_CreateFromNamedFile(rFilePath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &pKtxTexture) != KTX_SUCCESS)
		{
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Failed to read KTX texture (texture: %s)", rFilePath.c_str());
			return nullptr;
		}

		if (pKtxTexture->isCompressed)
		{
			ktxTexture_Destroy(pKtxTexture);
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Only uncompressed KTX formats are supported at the moment (texture: %s)", rFilePath.c_str()); // TODO:
			return nullptr;
		}

		if (pKtxTexture->classId != ktxTexture2_c)
		{
			ktxTexture_Destroy(pKtxTexture);
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Only KTX2 are supported at the moment (texture: %s)", rFilePath.c_str()); // TODO:
			return nullptr;
		}
		auto *pKtxTexture2 = reinterpret_cast<ktxTexture2 *>(pKtxTexture);

		FastCG::TextureFormat format;
		FastCG::TextureDataType dataType;
		FastCG::BitsPerChannel bitsPerChannel;
		if (!GetTextureFormatDataTypeAndBitsPerChannelFromKtxTexture2(pKtxTexture2, format, dataType, bitsPerChannel))
		{
			ktxTexture_Destroy(pKtxTexture);
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled KTX2 vulkan format (texture: %s, format: %d)", rFilePath.c_str(), (int)pKtxTexture2->vkFormat);
			return nullptr;
		}

		auto width = (uint32_t)pKtxTexture->baseWidth;
		auto height = (uint32_t)pKtxTexture->baseHeight;
		auto depth = (uint32_t)pKtxTexture->baseDepth;
		auto layers = (uint32_t)pKtxTexture->numLayers; // array
		auto mipCount = (uint32_t)pKtxTexture->numLevels;
		auto bytesPerPixel = (size_t)(bitsPerChannel.r + bitsPerChannel.g + bitsPerChannel.b + bitsPerChannel.a) >> 3;

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
				uint32_t mipWidth = width >> mip;
				uint32_t mipHeight = height >> mip;
				size_t mipSize = mipWidth * mipHeight * bytesPerPixel;
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

				size_t totalSize = width * height * depth * bytesPerPixel;
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

				auto sliceSize = width * height * bytesPerPixel;
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

				auto sliceSize = width * height * bytesPerPixel;
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
																	 bitsPerChannel,
																	 dataType,
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
		FastCG::BitsPerChannel bitsPerChannel;
		switch (componentCount)
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
			format = FastCG::TextureFormat::RGBA;
			bitsPerChannel = {8, 8, 8, 8};
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
																			   bitsPerChannel,
																			   FastCG::TextureDataType::UNSIGNED_CHAR,
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
