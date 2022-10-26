#ifndef FASTCG_BASE_BUFFER_H
#define FASTCG_BASE_BUFFER_H

#include <string>
#include <cstdint>

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
        DYNAMIC
    };

    struct BufferArgs
    {
        std::string name;
        BufferType type{BufferType::UNIFORM};
        BufferUsage usage{BufferUsage::DYNAMIC};
        size_t dataSize{0};
        const void *pData{nullptr};
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

    protected:
        std::string mName;
        BufferType mType;
        BufferUsage mUsage;

        BaseBuffer(const BufferArgs &rArgs) : mName(rArgs.name),
                                              mType(rArgs.type),
                                              mUsage(rArgs.usage)
        {
        }
        virtual ~BaseBuffer() = default;
    };

}

#endif