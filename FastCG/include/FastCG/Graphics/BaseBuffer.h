#ifndef FASTCG_BASE_BUFFER_H
#define FASTCG_BASE_BUFFER_H

#include <FastCG/Graphics/GraphicsEnums.h>

#include <vector>
#include <string>
#include <string.h>
#include <memory>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    struct VertexBindingDescriptor
    {
        uint32_t binding;
        uint32_t size;
        VertexDataType type;
        bool normalized;
        uint32_t stride;
        uint32_t offset;

        inline bool IsValid() const
        {
            return size > 0 && type != VertexDataType::NONE;
        }
    };

    struct BufferArgs
    {
        std::string name;
        BufferType type{BufferType::UNIFORM};
        BufferUsage usage{BufferUsage::DYNAMIC};
        size_t dataSize{0};
        const void *pData{nullptr};
        std::vector<VertexBindingDescriptor> vertexBindingDescriptors{};
    };

    class BaseBuffer
    {
    public:
        inline const std::string &GetName() const
        {
            return mName;
        }

        inline BufferType GetType() const
        {
            return mType;
        }

        inline BufferUsage GetUsage() const
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
        const BufferType mType;
        const BufferUsage mUsage;
        size_t mDataSize;
        std::unique_ptr<uint8_t[]> mpData;
        const std::vector<VertexBindingDescriptor> mVertexBindingDescriptors;

        BaseBuffer(const BufferArgs &rArgs) : mName(rArgs.name),
                                              mType(rArgs.type),
                                              mUsage(rArgs.usage),
                                              mDataSize(rArgs.dataSize),
                                              mVertexBindingDescriptors(rArgs.vertexBindingDescriptors)
        {
            assert(mType != BufferType::VERTEX_ATTRIBUTE || std::all_of(mVertexBindingDescriptors.cbegin(), mVertexBindingDescriptors.cend(), [](const auto &vbDesc)
                                                                        { return vbDesc.IsValid(); }));
            if (mDataSize > 0)
            {
                mpData = std::make_unique<uint8_t[]>(mDataSize);
                if (rArgs.pData != nullptr)
                {
                    memcpy((void *)mpData.get(), rArgs.pData, mDataSize);
                }
            }
        }
        virtual ~BaseBuffer() = default;
    };

}

#endif