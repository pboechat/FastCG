#if defined FASTCG_ANDROID
#include <FastCG/Platform/Android/AndroidApplication.h>
#endif
#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/Core/CollectionUtils.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Macros.h>

// defined in the application's Config.cpp
extern const char *const APPLICATION_NAME;

namespace FastCG
{
    AssetSystem::AssetSystem(const AssetSystemArgs &rArgs)
    {
        std::filesystem::path assetsRoot =
#if defined FASTCG_ANDROID
            // assets packaged in the APK are copied to the internal storage at app startup
            std::filesystem::path(AndroidApplication::GetInstance()->GetInternalDataPath()) / "assets"
#else
            // binaries are deployed to a sibling of the assets directory
            std::filesystem::current_path().parent_path() / "assets"
#endif
            ;
        mBundleRoots.emplace_back(assetsRoot / "FastCG");
        mBundleRoots.emplace_back(assetsRoot / APPLICATION_NAME);
        std::transform(rArgs.rBundles.cbegin(), rArgs.rBundles.cend(), std::back_inserter(mBundleRoots),
                       [&](const auto &bundle) { return assetsRoot / bundle; });
        CollectionUtils::RemoveDuplicates(mBundleRoots);
        FASTCG_LOG_DEBUG(AssetSystem, "Bundle roots:");
        for (const auto &rBundleRoot : mBundleRoots)
        {
            FASTCG_UNUSED(rBundleRoot);
            FASTCG_LOG_DEBUG(AssetSystem, "- %s", rBundleRoot.string().c_str());
        }
    }

}
