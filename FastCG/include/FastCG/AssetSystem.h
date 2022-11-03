#ifndef FASTCG_ASSET_SYSTEM
#define FASTCG_ASSET_SYSTEM

#include <FastCG/System.h>

#include <vector>
#include <string>

namespace FastCG
{
    struct AssetSystemArgs
    {
        const std::vector<std::string> &bundles;
    };

    class AssetSystem
    {
        FASTCG_DECLARE_SYSTEM(AssetSystem, AssetSystemArgs);

    public:
        inline std::vector<std::string> List(const std::string &rRelDirectoryPath, bool recursive = false) const;
        inline bool Resolve(const std::string &rRelFilePath, std::string &rAbsFilePath) const;
        inline std::string Resolve(const std::string &rRelFilePath) const;
        inline void Expand(const std::string &rRelFilePath, std::vector<std::string> &rAbsFilePaths, bool includeNonExistant = false) const;
        inline std::vector<std::string> Expand(const std::string &rRelFilePath, bool includeNonExistant = false) const;

        friend class BaseApplication;

    private:
        std::vector<std::string> mBundleRoots;

        AssetSystem(const AssetSystemArgs &rArgs);
    };

}

#include <FastCG/AssetSystem.inc>

#endif