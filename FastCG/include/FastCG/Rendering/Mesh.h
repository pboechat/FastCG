#ifndef FASTCG_MESH_H
#define FASTCG_MESH_H

#include <FastCG/Core/AABB.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/GraphicsUtils.h>

#include <cstdint>
#include <string>
#include <vector>

namespace FastCG
{
    struct VertexAttributeDescriptor
    {
        std::string name;
        BufferUsageFlags usage;
        size_t dataSize;
        const void *pData;
        std::vector<VertexBindingDescriptor> bindingDescriptors;
    };

    struct MeshArgs
    {
        std::string name;
        std::vector<VertexAttributeDescriptor> vertexAttributeDecriptors;
        struct
        {
            BufferUsageFlags usage;
            uint32_t count;
            const uint32_t *pData;
        } indices;
        AABB bounds{};
    };

    class Mesh final
    {
    public:
        Mesh(const MeshArgs &rArgs);
        ~Mesh();

        inline uint32_t GetVertexBufferCount() const
        {
            return (uint32_t)mVertexBuffers.size();
        }

        inline uint32_t GetIndexCount() const
        {
            return mIndexCount;
        }

        inline uint32_t GetTriangleCount() const
        {
            return mIndexCount / 3;
        }

        inline const std::string &GetName() const
        {
            return mName;
        }

        inline const Buffer *const *GetVertexBuffers() const
        {
            return mVertexBuffers.data();
        }

        inline const Buffer *GetIndexBuffer() const
        {
            return mpIndexBuffer;
        }

        inline const AABB &GetBounds() const
        {
            return mBounds;
        }

    protected:
        const std::string mName;
        std::vector<Buffer *> mVertexBuffers;
        Buffer *mpIndexBuffer{nullptr};
        uint32_t mIndexCount;
        const AABB mBounds;
    };

}

#endif
