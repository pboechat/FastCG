#ifndef FASTCG_BASE_MESH_H
#define FASTCG_BASE_MESH_H

#include <FastCG/Graphics/GraphicsEnums.h>
#include <FastCG/AABB.h>

#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace FastCG
{
	struct VertexAttributeDescriptor
	{
		std::string name;
		BufferUsage usage;
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
			BufferUsage usage;
			uint32_t count;
			const uint32_t *pData;
		} indices;
		AABB bounds{};
	};

	template <class BufferT>
	class BaseMesh
	{
	public:
		using Buffer = BufferT;

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

		BaseMesh(const MeshArgs &rArgs) : mName(rArgs.name),
										  mIndexCount(rArgs.indices.count),
										  mBounds(rArgs.bounds)
		{
			assert(std::none_of(mVertexBuffers.begin(), mVertexBuffers.end(), [](const auto *pBuffer)
								{ return pBuffer->GetVertexBindingDescriptors().empty(); }));
		}
		virtual ~BaseMesh() = default;
	};

}

#endif
