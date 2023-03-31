#include <FastCG/AssetSystem.h>

namespace FastCG
{
    AssetSystem::AssetSystem(const AssetSystemArgs &rArgs)
    {
        mBundleRoots.emplace_back("../assets/FastCG");
        std::transform(rArgs.rBundles.cbegin(), rArgs.rBundles.cend(), std::back_inserter(mBundleRoots), [](const auto &bundle)
                       { return "../assets/" + bundle; });
    }

}