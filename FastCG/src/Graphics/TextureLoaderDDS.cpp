#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Platform/FileReader.h>

#include <cstdint>
#include <memory>

// adapted from: https://learn.microsoft.com/en-us/windows/uwp/gaming/complete-code-for-ddstextureloader

// copied from ddsFormat.h
#define DDS_FORMAT_UNKNOWN 0
#define DDS_FORMAT_R32G32B32A32_SFLOAT 2
#define DDS_FORMAT_R32G32B32_SFLOAT 6
#define DDS_FORMAT_R16G16B16A16_UNORM 11
#define DDS_FORMAT_R32G32_SFLOAT 16
// not sure
#define DDS_FORMAT_A2R10G10B10_UNORM_PACK32 24
// not sure
#define DDS_FORMAT_B10G11R11_UFLOAT_PACK32 26
#define DDS_FORMAT_R8G8B8A8_UNORM 28
#define DDS_FORMAT_R16G16_SFLOAT 34
#define DDS_FORMAT_R16G16_UNORM 35
#define DDS_FORMAT_D32_SFLOAT 40
#define DDS_FORMAT_R32_SFLOAT 41
#define DDS_FORMAT_D24_UNORM_S8_UINT 45
#define DDS_FORMAT_X8_D24_UNORM_PACK32 46
#define DDS_FORMAT_R8G8_UNORM 49
#define DDS_FORMAT_R16_SFLOAT 54
#define DDS_FORMAT_D16_UNORM 55
#define DDS_FORMAT_R16_UNORM 56
#define DDS_FORMAT_R8_UNORM 61
#define DDS_FORMAT_BC1_RGBA_UNORM_BLOCK 71
#define DDS_FORMAT_BC2_UNORM_BLOCK 74
#define DDS_FORMAT_BC3_UNORM_BLOCK 77
#define DDS_FORMAT_BC4_UNORM_BLOCK 80
#define DDS_FORMAT_BC4_SNORM_BLOCK 81
#define DDS_FORMAT_BC5_UNORM_BLOCK 83
#define DDS_FORMAT_BC5_SNORM_BLOCK 84
#define DDS_FORMAT_B8G8R8A8_UNORM 87
#define DDS_FORMAT_B8G8R8_UNORM 88
#define DDS_FORMAT_BC6H_UFLOAT_BLOCK 95
#define DDS_FORMAT_BC6H_SFLOAT_BLOCK 96
#define DDS_FORMAT_BC7_UNORM_BLOCK 98

// copied from d3d11.h
#define DDS_RESOURCE_DIMENSION_UNKNOWN 0
#define DDS_RESOURCE_DIMENSION_BUFFER 1
#define DDS_RESOURCE_DIMENSION_TEXTURE1D 2
#define DDS_RESOURCE_DIMENSION_TEXTURE2D 3
#define DDS_RESOURCE_DIMENSION_TEXTURE3D 4
#define DDS_REQ_MIP_LEVELS 15
#define DDS_RESOURCE_MISC_TEXTURECUBE 0x4L
#define DDS_REQ_TEXTURE1D_U_DIMENSION 16384
#define DDS_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION 2048
#define DDS_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION 2048
#define DDS_REQ_TEXTURE2D_U_OR_V_DIMENSION 16384
#define DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION 2048
#define DDS_REQ_TEXTURECUBE_DIMENSION 16384

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                                                                 \
    ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) |                   \
     ((uint32_t)(uint8_t)(ch3) << 24))
#endif /* defined(MAKEFOURCC) */

#define DDS_MAGIC 0x20534444 // "DDS "

FASTCG_PACKED_PREFIX struct DDS_PIXELFORMAT
{
    uint32_t size;
    uint32_t flags;
    uint32_t fourCC;
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
} FASTCG_PACKED_SUFFIX;

#define DDS_FOURCC 0x00000004    // DDPF_FOURCC
#define DDS_RGB 0x00000040       // DDPF_RGB
#define DDS_RGBA 0x00000041      // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE 0x00020000 // DDPF_LUMINANCE

#define DDS_HEADER_FLAGS_TEXTURE 0x00001007 // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
#define DDS_HEADER_FLAGS_MIPMAP 0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME 0x00800000  // DDSD_DEPTH

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_WIDTH 0x00000004  // DDSD_WIDTH

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES                                                                                           \
    (DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX | DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |                   \
     DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ)

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

FASTCG_PACKED_PREFIX struct DDS_HEADER
{
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitchOrLinearSize;
    uint32_t depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32_t mipMapCount;
    uint32_t reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t caps;
    uint32_t caps2;
    uint32_t caps3;
    uint32_t caps4;
    uint32_t reserved2;
} FASTCG_PACKED_SUFFIX;

FASTCG_PACKED_PREFIX struct DDS_HEADER_DXT10
{
    uint32_t ddsFormat;
    uint32_t resourceDimension;
    uint32_t miscFlag; // see DDS_RESOURCE_MISC_FLAG
    uint32_t arraySize;
    uint32_t reserved;
} FASTCG_PACKED_SUFFIX;

#define ISBITMASK(r, g, b, a) (ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a)

uint32_t GetDDSFormat(const DDS_PIXELFORMAT &ddpf)
{
    if (ddpf.flags & DDS_RGB)
    {
        // note that sRGB formats are written using the "DX10" extended header

        switch (ddpf.RGBBitCount)
        {
        case 32:
            if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
            {
                return DDS_FORMAT_R8G8B8A8_UNORM;
            }
            else if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
            {
                return DDS_FORMAT_B8G8R8A8_UNORM;
            }
            else if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
            {
                return DDS_FORMAT_B8G8R8_UNORM;
            }

            // no DDS format maps to ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000) aka D3DFMT_X8B8G8R8

            // note that many common DDS reader/writers (including D3DX) swap the
            // the RED/BLUE masks for 10:10:10:2 formats. We assumme
            // below that the 'backwards' header mask is being used since it is most
            // likely written by D3DX. The more robust solution is to use the 'DX10'
            // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

            // for 'correct' writers, this should be 0x000003ff, 0x000ffc00, 0x3ff00000 for RGB data
            else if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
            {
                return DDS_FORMAT_A2R10G10B10_UNORM_PACK32;
            }
            // no DDS format maps to ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000) aka D3DFMT_A2R10G10B10
            else if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
            {
                return DDS_FORMAT_R16G16_UNORM;
            }
            else if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
            {
                // only 32-bit color channel format in D3D9 was R32F.
                return DDS_FORMAT_R32_SFLOAT; // D3DX writes this out as a FourCC of 114.
            }
            break;

        case 24:
            // no 24bpp DDS formats aka D3DFMT_R8G8B8
            break;

        case 16:
            // no 16bpp BGR/BGRA DDS formats eg D3DFMT_B5G5R5A1
            break;
        }
    }
    else if (ddpf.flags & DDS_LUMINANCE)
    {
        if (ddpf.RGBBitCount == 8)
        {
            if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
            {
                return DDS_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // no DDS format maps to ISBITMASK(0x0f, 0x00, 0x00, 0xf0) aka D3DFMT_A4L4.
        }
        else if (ddpf.RGBBitCount == 16)
        {
            if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
            {
                return DDS_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
            else if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
            {
                return DDS_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
        }
    }
    else if (ddpf.flags & DDS_FOURCC)
    {
        if (ddpf.fourCC == MAKEFOURCC('D', 'X', 'T', '1'))
        {
            return DDS_FORMAT_BC1_RGBA_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('D', 'X', 'T', '3'))
        {
            return DDS_FORMAT_BC2_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('D', 'X', 'T', '5'))
        {
            return DDS_FORMAT_BC3_UNORM_BLOCK;
        }
        // while pre-mulitplied alpha isn't directly supported by the DDS formats,
        // they are basically the same as these BC formats so they can be mapped
        else if (ddpf.fourCC == MAKEFOURCC('D', 'X', 'T', '2'))
        {
            return DDS_FORMAT_BC2_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('D', 'X', 'T', '4'))
        {
            return DDS_FORMAT_BC3_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('A', 'T', 'I', '1'))
        {
            return DDS_FORMAT_BC4_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('B', 'C', '4', 'U'))
        {
            return DDS_FORMAT_BC4_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('B', 'C', '4', 'S'))
        {
            return DDS_FORMAT_BC4_SNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('A', 'T', 'I', '2'))
        {
            return DDS_FORMAT_BC5_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('B', 'C', '5', 'U'))
        {
            return DDS_FORMAT_BC5_UNORM_BLOCK;
        }
        else if (ddpf.fourCC == MAKEFOURCC('B', 'C', '5', 'S'))
        {
            return DDS_FORMAT_BC5_SNORM_BLOCK;
        }

        // BC6H and BC7 are written using the "DX10" extended header

        // no RGBG/GRGB DDS formats

        // check for D3DFORMAT enums being set here
        switch (ddpf.fourCC)
        {
        case 36: // D3DFMT_A16B16G16R16
            return DDS_FORMAT_R16G16B16A16_UNORM;

        case 110: // no D3DFMT_Q16W16V16U16
            break;

        case 111: // D3DFMT_R16F
            return DDS_FORMAT_R16_SFLOAT;

        case 112: // D3DFMT_G16R16F
            return DDS_FORMAT_R16G16_SFLOAT;

        case 113: // no D3DFMT_A16B16G16R16F
            break;

        case 114: // D3DFMT_R32F
            return DDS_FORMAT_R32_SFLOAT;

        case 115: // D3DFMT_G32R32F
            return DDS_FORMAT_R32G32_SFLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return DDS_FORMAT_R32G32B32A32_SFLOAT;
        }
    }

    return DDS_FORMAT_UNKNOWN;
}

FastCG::TextureFormat GetTextureFormat(uint32_t ddsFormat)
{
#ifdef CASE_RETURN
#undef CASE_RETURN
#endif

#define CASE_RETURN(format)                                                                                            \
    case DDS_FORMAT_##format:                                                                                          \
        return FastCG::TextureFormat::format

    switch (ddsFormat)
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
    }

    FASTCG_THROW_EXCEPTION(FastCG::Exception, "Can't get texture format (ddsFormat: %d)", ddsFormat);
    return (FastCG::TextureFormat)0;

#undef CASE_RETURN
}

namespace FastCG
{
    Texture *TextureLoader::LoadDDS(const std::filesystem::path &rFilePath, TextureLoadSettings settings)
    {
        size_t ddsDataSize;
        auto ddsData = FileReader::ReadBinary(rFilePath, ddsDataSize);

        // validate DDS file in memory
        if (ddsDataSize < (sizeof(uint32_t) + sizeof(DDS_HEADER)))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't parse the DDS data (texture: %s)", rFilePath.string().c_str());
        }

        uint32_t dwMagicNumber = *(const uint32_t *)(&ddsData[0]);
        if (dwMagicNumber != DDS_MAGIC)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't verify DDS magic number (texture: %s)",
                                   rFilePath.string().c_str());
        }

        const auto *header = reinterpret_cast<const DDS_HEADER *>(&ddsData[0] + sizeof(uint32_t));

        // verify the header to validate the DDS file
        if (header->size != sizeof(DDS_HEADER) || header->ddspf.size != sizeof(DDS_PIXELFORMAT))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't parse the DDS header (texture: %s)",
                                   rFilePath.string().c_str());
        }

        // check for the DX10 extension
        bool bDXT10Header = false;
        if ((header->ddspf.flags & DDS_FOURCC) != 0 && header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0'))
        {
            // must be long enough for both headers and magic value
            if (ddsDataSize < (sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10)))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't parse the DDS DX10 header (texture: %s)",
                                       rFilePath.string().c_str());
            }

            bDXT10Header = true;
        }

        ptrdiff_t dataOffset = sizeof(uint32_t) + sizeof(DDS_HEADER) + (bDXT10Header ? sizeof(DDS_HEADER_DXT10) : 0);

        auto *pData = ddsData.get() + dataOffset;

        auto width = header->width;
        auto height = header->height;
        uint32_t resDim = DDS_RESOURCE_DIMENSION_UNKNOWN;
        uint32_t depthOrSlices = 1;
        uint32_t ddsFormat = DDS_FORMAT_UNKNOWN;
        bool isCubeMap = false;

        auto mipCount = (uint8_t)header->mipMapCount;
        if (mipCount == 0)
        {
            mipCount = 1;
        }

        if ((header->ddspf.flags & DDS_FOURCC) && header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0'))
        {
            const auto *d3d10Header =
                reinterpret_cast<const DDS_HEADER_DXT10 *>((const char *)header + sizeof(DDS_HEADER));

            depthOrSlices = d3d10Header->arraySize;
            if (depthOrSlices == 0)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Invalid array size (texture: %s)", rFilePath.string().c_str());
            }

            ddsFormat = d3d10Header->ddsFormat;

            switch (d3d10Header->resourceDimension)
            {
            case DDS_RESOURCE_DIMENSION_TEXTURE1D:
                // D3DX writes 1D textures with a fixed height of 1
                if ((header->flags & DDS_HEIGHT) != 0 && height != 1)
                {
                    FASTCG_THROW_EXCEPTION(Exception, "Invalid 1D texture height (texture: %s)",
                                           rFilePath.string().c_str());
                }
                height = depthOrSlices = 1;
                break;
            case DDS_RESOURCE_DIMENSION_TEXTURE2D:
                if (d3d10Header->miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE)
                {
                    depthOrSlices = 6;
                    isCubeMap = true;
                }
                else
                {
                    depthOrSlices = 1;
                }
                break;
            case DDS_RESOURCE_DIMENSION_TEXTURE3D:
                if ((header->flags & DDS_HEADER_FLAGS_VOLUME) == 0)
                {
                    FASTCG_THROW_EXCEPTION(Exception, "Missing volume flag in 3D texture flags (texture: %s)",
                                           rFilePath.string().c_str());
                }
                depthOrSlices = header->depth;
                break;
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Invalid resource dimension (texture: %s, resDim: %d)",
                                       rFilePath.string().c_str(), resDim);
                return nullptr;
            }

            resDim = d3d10Header->resourceDimension;
        }
        else
        {
            ddsFormat = GetDDSFormat(header->ddspf);

            if (ddsFormat == DDS_FORMAT_UNKNOWN)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Unknown DDS format (texture %s)", rFilePath.string().c_str());
                return nullptr;
            }
            else if (header->flags & DDS_HEADER_FLAGS_VOLUME)
            {
                resDim = DDS_RESOURCE_DIMENSION_TEXTURE3D;
            }
            else
            {
                if ((header->caps2 & DDS_CUBEMAP) != 0)
                {
                    // we require all six faces to be defined
                    if ((header->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                    {
                        FASTCG_THROW_EXCEPTION(Exception, "Missing faces in cubemap texture (texture: %s)",
                                               rFilePath.string().c_str());
                        return nullptr;
                    }

                    depthOrSlices = 6;
                    isCubeMap = true;
                }

                resDim = DDS_RESOURCE_DIMENSION_TEXTURE2D;

                // note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture.
            }
        }

        auto format = ::GetTextureFormat(ddsFormat);

        // bound sizes (for security purposes, we don't trust DDS file metadata larger than the D3D 11.x hardware
        // requirements)
        if (mipCount > DDS_REQ_MIP_LEVELS)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Invalid number of mips (texture: %s, mipCount: %d)",
                                   rFilePath.string().c_str(), mipCount);
            return nullptr;
        }

        TextureType type;
        switch (resDim)
        {
        case DDS_RESOURCE_DIMENSION_TEXTURE1D:
            if (width > DDS_REQ_TEXTURE1D_U_DIMENSION)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Invalid 1D texture dimensions (texture: %s)",
                                       rFilePath.string().c_str());
                return nullptr;
            }
            type = TextureType::TEXTURE_1D;
            break;
        case DDS_RESOURCE_DIMENSION_TEXTURE2D:
            if (isCubeMap)
            {
                if (width > DDS_REQ_TEXTURECUBE_DIMENSION || height > DDS_REQ_TEXTURECUBE_DIMENSION)
                {
                    FASTCG_THROW_EXCEPTION(Exception, "Invalid cubemap dimensions (texture: %s)",
                                           rFilePath.string().c_str());
                    return nullptr;
                }
                type = TextureType::TEXTURE_CUBE_MAP;
            }
            else
            {
                if (width > DDS_REQ_TEXTURE2D_U_OR_V_DIMENSION || height > DDS_REQ_TEXTURE2D_U_OR_V_DIMENSION)
                {
                    FASTCG_THROW_EXCEPTION(Exception, "Invalid 2D texture dimensions (texture: %s)",
                                           rFilePath.string().c_str());
                    return nullptr;
                }
                if (depthOrSlices > 1)
                {
                    type = TextureType::TEXTURE_2D_ARRAY;
                }
                else
                {
                    type = TextureType::TEXTURE_2D;
                }
            }
            break;
        case DDS_RESOURCE_DIMENSION_TEXTURE3D:
            if (width > DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION || height > DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION ||
                depthOrSlices > DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Invalid 3D textures dimensions (texture: %s)",
                                       rFilePath.string().c_str());
                return nullptr;
            }
            type = TextureType::TEXTURE_3D;
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Invalid resource dimension (texture: %s, resDim: %d)",
                                   rFilePath.string().c_str(), resDim);
        }

        return GraphicsSystem::GetInstance()->CreateTexture({rFilePath.stem().string(), width, height, depthOrSlices,
                                                             mipCount, type, settings.usage, format, settings.filter,
                                                             settings.wrapMode, pData});
    }
}
