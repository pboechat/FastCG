#ifndef FASTCG_BASE_BUFFER_H
#define FASTCG_BASE_BUFFER_H

#include <FastCG/Graphics/GraphicsUtils.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace FastCG
{
    class BaseBuffer
    {
    public:
        struct Args
        {
            std::string name;
            BufferUsageFlags usage;
            size_t dataSize;
            const void *pData;
            std::vector<VertexBindingDescriptor> vertexBindingDescriptors;

            Args(const std::string &rName = "", BufferUsageFlags usage = BufferUsageFlagBit::UNIFORM,
                 size_t dataSize = 0, const void *pData = nullptr,
                 const std::vector<VertexBindingDescriptor> &rVertexBindingDescriptors = {})
                : name(rName), usage(usage), dataSize(dataSize), pData(pData),
                  vertexBindingDescriptors(rVertexBindingDescriptors)
            {
            }
        };

        inline const std::string &GetName() const
        {
            return mName;
        }

        inline BufferUsageFlags GetUsage() const
        {
            return mUsage;
        }

        inline size_t GetDataSize() const
        {
            return mDataSize;
        }

        inline const uint8_t *GetData() const
        {
            return mpData.get();
        }

        inline const std::vector<VertexBindingDescriptor> &GetVertexBindingDescriptors() const
        {
            return mVertexBindingDescriptors;
        }

    protected:
        const std::string mName;
        const BufferUsageFlags mUsage;
        size_t mDataSize;
        std::unique_ptr<uint8_t[]> mpData;
        const std::vector<VertexBindingDescriptor> mVertexBindingDescriptors;

        BaseBuffer(const Args &rArgs)
            : mName(rArgs.name), mUsage(rArgs.usage), mDataSize(rArgs.dataSize),
              mVertexBindingDescriptors(rArgs.vertexBindingDescriptors)
        {
            assert((mUsage & BufferUsageFlagBit::VERTEX_BUFFER) == 0 ||
                   std::all_of(mVertexBindingDescriptors.cbegin(), mVertexBindingDescriptors.cend(),
                               [](const auto &vbDesc) { return vbDesc.IsValid(); }));
            if (mDataSize > 0)
            {
                mpData = std::make_unique<uint8_t[]>(mDataSize);
                if (rArgs.pData != nullptr)
                {
                    std::memcpy((void *)mpData.get(), rArgs.pData, mDataSize);
                }
            }
        }
        virtual ~BaseBuffer() = default;
    };

}

#endif