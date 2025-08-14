#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/TextureLoader.h>

#include <ktx.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

// copied from vulkan_core.h
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

FastCG::TextureFormat GetTextureFormat(ktxTexture2 *pKtxTexture2)
{
#ifdef CASE_RETURN
#undef CASE_RETURN
#endif
#define CASE_RETURN(format)                                                                                            \
    case KTX2_VK_FORMAT_##format:                                                                                      \
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

namespace FastCG
{
    Texture *TextureLoader::LoadKTX(const std::filesystem::path &rFilePath, TextureLoadSettings settings)
    {
        ktxTexture *pKtxTexture;

        if (ktxTexture_CreateFromNamedFile(rFilePath.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                           &pKtxTexture) != KTX_SUCCESS)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't create KTX texture (texture: %s)", rFilePath.string().c_str());
            return nullptr;
        }

        if (pKtxTexture->classId != ktxTexture2_c)
        {
            ktxTexture_Destroy(pKtxTexture);
            FASTCG_THROW_EXCEPTION(Exception, "Only KTX2 are supported at the moment (texture: %s)",
                                   rFilePath.string().c_str()); // TODO:
            return nullptr;
        }
        auto *pKtxTexture2 = reinterpret_cast<ktxTexture2 *>(pKtxTexture);

        auto format = ::GetTextureFormat(pKtxTexture2);
        auto width = (uint32_t)pKtxTexture->baseWidth;
        auto height = (uint32_t)pKtxTexture->baseHeight;
        auto depth = (uint32_t)pKtxTexture->baseDepth;
        auto layers = (uint32_t)pKtxTexture->numLayers; // array
        auto mipCount = (uint32_t)pKtxTexture->numLevels;

        TextureType type;
        uint32_t depthOrSlices;

        std::unique_ptr<uint8_t[]> data;
        if ((pKtxTexture->numDimensions == 1 || pKtxTexture->numDimensions == 2) && !pKtxTexture->isArray &&
            !pKtxTexture->isCubemap)
        {
            assert(layers == 1);
            type = pKtxTexture->numDimensions == 1 ? TextureType::TEXTURE_1D : TextureType::TEXTURE_2D;
            // force slices to 1
            // TODO: support 3D/array/cubemap mips
            depthOrSlices = 1;

            struct MipStats
            {
                uint32_t width;
                uint32_t height;
                size_t size;
            };
            size_t texelCount = 0;
            std::vector<MipStats> mipStats(mipCount);
            for (uint32_t mip = 0; mip < mipCount; ++mip)
            {
                auto mipWidth = width >> mip;
                auto mipHeight = height >> mip;
                auto mipSize = GetTextureDataSize(format, mipWidth, mipHeight, 1);
                mipStats[mip] = {mipWidth, mipHeight, mipSize};
                texelCount += mipSize;
            }
            data = std::make_unique<uint8_t[]>(texelCount);
            auto *pData = &data[0];
            for (uint32_t mip = 0; mip < mipCount; ++mip)
            {
                ktx_size_t mipOffset;
                ktxTexture_GetImageOffset(pKtxTexture, (ktx_uint32_t)mip, 0, 0, &mipOffset);
                std::memcpy(pData, pKtxTexture->pData + mipOffset, mipStats[mip].size);
                pData += mipStats[mip].size;
            }
        }
        else
        {
            if (pKtxTexture->numDimensions == 3)
            {
                type = TextureType::TEXTURE_3D;
                // force mipCount to 1
                // TODO: support 3D mips
                mipCount = 1;
                // TODO: support 3D arrays
                depthOrSlices = depth;

                size_t texelCount = GetTextureDataSize(format, width, height, depth);
                data = std::make_unique<uint8_t[]>(texelCount);
                ktx_size_t offset;
                ktxTexture_GetImageOffset(pKtxTexture, 0, 0, 0, &offset);
                std::memcpy(&data[0], pKtxTexture->pData + offset, texelCount);
            }
            else if (pKtxTexture->isCubemap)
            {
                type = TextureType::TEXTURE_CUBE_MAP;
                // force mipCount to 1
                // TODO: support cubemap mips
                mipCount = 1;
                assert(depth == 1);
                assert(pKtxTexture->numFaces == 6); // FIXME: invariant checking
                depthOrSlices = layers = 6;

                auto sliceSize = GetTextureDataSize(format, width, height, 1);
                data = std::make_unique<uint8_t[]>(sliceSize * layers);
                auto *pData = &data[0];
                for (uint32_t slice = 0; slice < layers; ++slice)
                {
                    ktx_size_t sliceOffset;
                    ktxTexture_GetImageOffset(pKtxTexture, 0, 0, (ktx_uint32_t)slice, &sliceOffset);
                    std::memcpy(pData, pKtxTexture->pData + sliceOffset, sliceSize);
                    pData += sliceSize;
                }
            }
            else
            {
                assert(pKtxTexture->isArray);
                assert(pKtxTexture->numDimensions == 2);
                type = TextureType::TEXTURE_2D_ARRAY;
                // force mipCount to 1
                // TODO: support 3D mips
                mipCount = 1;
                // force depth to 1
                // TODO: support 3D arrays
                depth = 1;
                depthOrSlices = layers;

                auto sliceSize = GetTextureDataSize(format, width, height, 1);
                data = std::make_unique<uint8_t[]>(sliceSize * layers);
                auto *pData = &data[0];
                for (uint32_t slice = 0; slice < layers; ++slice)
                {
                    ktx_size_t sliceOffset;
                    ktxTexture_GetImageOffset(pKtxTexture, 0, slice, 0, &sliceOffset);
                    std::memcpy(pData, pKtxTexture->pData + sliceOffset, sliceSize);
                    pData += sliceSize;
                }
            }
        }

        ktxTexture_Destroy(pKtxTexture);

        return GraphicsSystem::GetInstance()->CreateTexture({rFilePath.stem().string(), width, height, depthOrSlices,
                                                             (uint8_t)mipCount, type, settings.usage, format,
                                                             settings.filter, settings.wrapMode, &data[0]});
    }

}
