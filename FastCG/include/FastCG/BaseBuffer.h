#ifndef FASTCG_BASE_BUFFER_H
#define FASTCG_BASE_BUFFER_H

#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    enum class BufferType : uint8_t
    {
        UNIFORM = 0,
        VERTEX_ATTRIBUTE,
        INDICES
    };

    enum class BufferUsage : uint8_t
    {
        STATIC = 0,
        DYNAMIC,
        STREAM
    };

    enum class VertexDataType : uint8_t
    {
        NONE = 0,
        FLOAT,
        UNSIGNED_BYTE
    };

    struct VertexBindingDescriptor
    {
        uint32_t binding;
        uint32_t size;
        VertexDataType type;
        bool normalized;
        uint32_t stride;
        uint32_t offset;

        bool IsValid() const
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

        inline const std::vector<VertexBindingDescriptor> &GetVertexBindingDescriptors() const
        {
            return mVertexBindingDescriptors;
        }

    protected:
        const std::string mName;
        const BufferType mType;
        const BufferUsage mUsage;
        const std::vector<VertexBindingDescriptor> mVertexBindingDescriptors;

        BaseBuffer(const BufferArgs &rArgs) : mName(rArgs.name),
                                              mType(rArgs.type),
                                              mUsage(rArgs.usage),
                                              mVertexBindingDescriptors(rArgs.vertexBindingDescriptors)
        {
            assert(mType != BufferType::VERTEX_ATTRIBUTE || std::all_of(mVertexBindingDescriptors.cbegin(), mVertexBindingDescriptors.cend(), [](const auto &vbDesc)
                                                                        { return vbDesc.IsValid(); }));
        }
        virtual ~BaseBuffer() = default;
    };

}

#endif