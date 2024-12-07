#ifndef FASTCG_ASSET_SYSTEM_H
#define FASTCG_ASSET_SYSTEM_H

#include <FastCG/Core/System.h>

#include <filesystem>
#include <vector>

namespace FastCG
{
    struct AssetSystemArgs
    {
        const std::vector<std::filesystem::path> &rBundles;
    };

    class AssetSystem
    {
        FASTCG_DECLARE_SYSTEM(AssetSystem, AssetSystemArgs);

    public:
        inline std::vector<std::filesystem::path> List(const std::filesystem::path &rRelDirectoryPath,
                                                       bool recursive = false) const;
        inline bool Resolve(const std::filesystem::path &rRelFilePath, std::filesystem::path &rAbsFilePath) const;
        inline std::filesystem::path Resolve(const std::filesystem::path &rRelFilePath) const;
        inline void Expand(const std::filesystem::path &rRelFilePath, std::vector<std::filesystem::path> &rAbsFilePaths,
                           bool includeNonExistant = false) const;
        inline std::vector<std::filesystem::path> Expand(const std::filesystem::path &rRelFilePath,
                                                         bool includeNonExistant = false) const;

    private:
        std::vector<std::filesystem::path> mBundleRoots;

        AssetSystem(const AssetSystemArgs &rArgs);
    };

}

#include <FastCG/Assets/AssetSystem.inc>

#endif