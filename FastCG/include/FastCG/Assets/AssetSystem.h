#ifndef FASTCG_ASSET_SYSTEM_H
#define FASTCG_ASSET_SYSTEM_H

#include <FastCG/Core/System.h>

#include <string>
#include <vector>

namespace FastCG
{
    struct AssetSystemArgs
    {
        const std::vector<std::string> &rBundles;
    };

    class AssetSystem
    {
        FASTCG_DECLARE_SYSTEM(AssetSystem, AssetSystemArgs);

    public:
        inline std::vector<std::string> List(const std::string &rRelDirectoryPath, bool recursive = false) const;
        inline bool Resolve(const std::string &rRelFilePath, std::string &rAbsFilePath) const;
        inline std::string Resolve(const std::string &rRelFilePath) const;
        inline void Expand(const std::string &rRelFilePath, std::vector<std::string> &rAbsFilePaths,
                           bool includeNonExistant = false) const;
        inline std::vector<std::string> Expand(const std::string &rRelFilePath, bool includeNonExistant = false) const;

    private:
        std::vector<std::string> mBundleRoots;

        AssetSystem(const AssetSystemArgs &rArgs);
    };

}

#include <FastCG/Assets/AssetSystem.inc>

#endif