#include <FastCG/AssetSystem.h>

namespace FastCG
{
    AssetSystem::AssetSystem(const AssetSystemArgs &rArgs)
    {
        mBundleRoots.emplace_back("../assets/FastCG");
        std::transform(rArgs.bundles.cbegin(), rArgs.bundles.cend(), std::back_inserter(mBundleRoots), [](const auto &bundle)
                       { return "../assets/" + bundle; });
    }

}