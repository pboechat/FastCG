#ifndef FASTCG_BASE_MESH_H
#define FASTCG_BASE_MESH_H

#include <FastCG/BaseBuffer.h>
#include <FastCG/AABB.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace FastCG
{
	struct VertexAttributeDescriptor
	{
		std::string name;
		BufferUsage usage;
		size_t dataSize;
		void *pData;
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
			uint32_t *pData;
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
		}
		virtual ~BaseMesh() = default;
	};

}

#endif
