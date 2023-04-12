#include <cassert>
#include <algorithm>

namespace FastCG
{
    template <class CollectionT>
    void CollectionUtils::Slice(const CollectionT &rSrcCollection, size_t start, size_t end, CollectionT &rDstCollection)
    {
        assert(end > start);
        rDstCollection.resize(end - start);
        std::copy(rSrcCollection.cbegin() + start, rSrcCollection.cbegin() + end, rDstCollection.begin());
    }

}