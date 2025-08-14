#include <algorithm>
#include <cassert>
#include <set>

namespace FastCG
{
    template <class CollectionT>
    void CollectionUtils::Slice(const CollectionT &rSrcCollection, size_t start, size_t end,
                                CollectionT &rDstCollection)
    {
        assert(end > start);
        rDstCollection.resize(end - start);
        std::copy(rSrcCollection.cbegin() + start, rSrcCollection.cbegin() + end, rDstCollection.begin());
    }

    template <typename T>
    void CollectionUtils::RemoveDuplicates(std::vector<T> &rVector)
    {
        std::set<T> seen;
        rVector.erase(std::remove_if(rVector.begin(), rVector.end(),
                                     [&seen](const T &value) { return !seen.insert(value).second; }),
                      rVector.end());
    }

}