#ifndef FASTCG_BASE_TEXTURE_H
#define FASTCG_BASE_TEXTURE_H

#include <FastCG/Core/Exception.h>
#include <FastCG/Graphics/GraphicsUtils.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace FastCG
{
    class BaseTexture
    {
    public:
        struct Args
        {
            std::string name;
            uint32_t width;
            uint32_t height;
            uint32_t depthOrSlices; // depth if 3D and slices if array/cubemap
            uint8_t mipCount;
            TextureType type;
            TextureUsageFlags usage;
            TextureFormat format;
            TextureFilter filter;
            TextureWrapMode wrapMode;
            const uint8_t *pData;

            Args(const std::string &rName = "", uint32_t width = 1, uint32_t height = 1, uint32_t depthOrSlices = 1,
                 uint8_t mipCount = 1, TextureType type = TextureType::TEXTURE_2D,
                 TextureUsageFlags usage = TextureUsageFlagBit::SAMPLED,
                 TextureFormat format = TextureFormat::R8G8B8A8_UNORM,
                 TextureFilter filter = TextureFilter::LINEAR_FILTER, TextureWrapMode wrapMode = TextureWrapMode::CLAMP,
                 const uint8_t *pData = nullptr)
                : name(rName), width(width), height(height), depthOrSlices(depthOrSlices), mipCount(mipCount),
                  type(type), usage(usage), format(format), filter(filter), wrapMode(wrapMode), pData(pData)
            {
            }
        };

        inline const std::string &GetName() const
        {
            return mName;
        }

        inline uint32_t GetHeight() const
        {
            return mHeight;
        }

        inline uint32_t GetWidth() const
        {
            return mWidth;
        }

        inline uint32_t GetDepth() const
        {
            return mType == TextureType::TEXTURE_3D ? mDepthOrSlices : 1;
        }

        inline uint32_t GetHeight(uint8_t mip) const
        {
            return std::max((uint32_t)1, (uint32_t)mHeight >> mip);
        }

        inline uint32_t GetWidth(uint8_t mip) const
        {
            return std::max((uint32_t)1, (uint32_t)mWidth >> mip);
        }

        inline uint32_t GetDepth(uint8_t mip) const
        {
            return std::max((uint32_t)1, (uint32_t)GetDepth() >> mip);
        }

        inline uint32_t GetSlices() const
        {
            // TODO: support arrays for other texture types
            return mType == TextureType::TEXTURE_2D_ARRAY || mType == TextureType::TEXTURE_CUBE_MAP ? mDepthOrSlices
                                                                                                    : 1;
        }

        inline uint8_t GetMipCount() const
        {
            // TODO: support mips for other texture types
            return mType == TextureType::TEXTURE_1D || mType == TextureType::TEXTURE_2D ? mMipCount : 1;
        }

        inline TextureType GetType() const
        {
            return mType;
        }

        inline TextureUsageFlags GetUsage() const
        {
            return mUsage;
        }

        inline TextureFormat GetFormat() const
        {
            return mFormat;
        }

        inline TextureFilter GetFilter() const
        {
            return mFilter;
        }

        inline TextureWrapMode GetWrapMode() const
        {
            return mWrapMode;
        }

        inline size_t GetBaseMipDataSize() const
        {
            return GetTextureDataSize(GetFormat(), GetWidth(), GetHeight(), GetDepth());
        }

        inline size_t GetMipDataSize(uint8_t mip) const
        {
            return GetTextureDataSize(GetFormat(), GetWidth(mip), GetHeight(mip), GetDepth(mip));
        }

        inline size_t GetSliceDataSize() const
        {
            return GetBaseMipDataSize();
        }

        inline size_t GetDataSize() const
        {
            // TODO: support mips for other texture types
            switch (mType)
            {
            case TextureType::TEXTURE_1D:
            case TextureType::TEXTURE_2D: {
                size_t totalSize = 0;
                for (uint8_t mip = 0; mip < mMipCount; ++mip)
                {
                    totalSize += GetMipDataSize(mip);
                }
                return totalSize;
            }
            case TextureType::TEXTURE_3D:
            case TextureType::TEXTURE_CUBE_MAP:
            case TextureType::TEXTURE_2D_ARRAY:
                return GetSliceDataSize() * mDepthOrSlices;
            default:
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't get data size (type: %s)", GetTextureTypeString(mType));
                return 0;
            }
        }

        inline const uint8_t *GetData() const
        {
            return mpData.get();
        }

    protected:
        const std::string mName;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mDepthOrSlices; // depth if 3D and slices if array/cubemap
        uint8_t mMipCount;
        TextureType mType;
        TextureUsageFlags mUsage;
        TextureFormat mFormat;
        TextureFilter mFilter;
        TextureWrapMode mWrapMode;
        std::unique_ptr<uint8_t[]> mpData;

        BaseTexture(const Args &rArgs)
            : mName(rArgs.name), mWidth(rArgs.width), mHeight(rArgs.height), mDepthOrSlices(rArgs.depthOrSlices),
              mMipCount(rArgs.mipCount), mType(rArgs.type), mUsage(rArgs.usage), mFormat(rArgs.format),
              mFilter(rArgs.filter), mWrapMode(rArgs.wrapMode)
        {
            assert(mHeight == 1 || mType != TextureType::TEXTURE_1D);
            assert(mDepthOrSlices == 1 || mType == TextureType::TEXTURE_3D || mType == TextureType::TEXTURE_CUBE_MAP ||
                   mType == TextureType::TEXTURE_2D_ARRAY); // TODO: support arrays for other texture types
            assert(mDepthOrSlices == 6 || mType != TextureType::TEXTURE_CUBE_MAP);
            assert(mMipCount == 1 || mType == TextureType::TEXTURE_1D ||
                   mType == TextureType::TEXTURE_2D); // TODO: support mips for other texture types
            if (rArgs.pData != nullptr)
            {
                auto dataSize = GetDataSize();
                mpData = std::make_unique<uint8_t[]>(dataSize);
                std::memcpy((void *)mpData.get(), rArgs.pData, dataSize);
            }
        }
        virtual ~BaseTexture() = default;
    };

}

#endif
