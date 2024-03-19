#if defined FASTCG_ANDROID
#include <FastCG/Platform/Android/AndroidApplication.h>
#endif
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/CollectionUtils.h>
#include <FastCG/Assets/AssetSystem.h>

// defined in the application's Config.cpp
extern const char *const APPLICATION_NAME;

namespace FastCG
{
    AssetSystem::AssetSystem(const AssetSystemArgs &rArgs)
    {
        std::string assetsRoot =
#if defined FASTCG_ANDROID
            // assets packaged in the APK are copied to the internal storage at app startup
            AndroidApplication::GetInstance()->GetInternalDataPath() + std::string("/assets")
#else
            // binaries are deployed to a sibling of the assets directory
            "../assets"
#endif
            ;
        mBundleRoots.emplace_back(assetsRoot + "/FastCG");
        mBundleRoots.emplace_back(assetsRoot + "/" + APPLICATION_NAME);
        std::transform(rArgs.rBundles.cbegin(), rArgs.rBundles.cend(), std::back_inserter(mBundleRoots), [&](const auto &bundle)
                       { return assetsRoot + "/" + bundle; });
        CollectionUtils::RemoveDuplicates(mBundleRoots);
        FASTCG_LOG_DEBUG(AssetSystem, "Bundle roots:");
        for (const auto &rBundleRoot : mBundleRoots)
        {
            FASTCG_UNUSED(rBundleRoot);
            FASTCG_LOG_DEBUG(AssetSystem, "- %s", rBundleRoot.c_str());
        }
    }

}
