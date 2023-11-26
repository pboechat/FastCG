#ifndef FASTCG_COLLECTION_UTILS_H
#define FASTCG_COLLECTION_UTILS_H

namespace FastCG
{
	class CollectionUtils final
	{
	public:
		template <class CollectionT>
		inline static void Slice(const CollectionT &rSrcCollection, size_t start, size_t end, CollectionT &rDstCollection);

	private:
		CollectionUtils() = delete;
		~CollectionUtils() = delete;
	};

}

#include <FastCG/Core/CollectionUtils.inc>

#endif
