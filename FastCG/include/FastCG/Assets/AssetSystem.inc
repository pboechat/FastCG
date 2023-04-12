#include <FastCG/Platform/File.h>
#include <FastCG/Platform/Directory.h>

#include <algorithm>

namespace FastCG
{
    std::vector<std::string> AssetSystem::List(const std::string &rRelDirectoryPath, bool recursive /* = false*/) const
    {
        std::vector<std::string> fileList;
        for (const auto &bundleRoot : mBundleRoots)
        {
            auto absDirPath = bundleRoot + "/" + rRelDirectoryPath;
            auto partialFileList = Directory::List(absDirPath, recursive);
            fileList.insert(fileList.end(), partialFileList.begin(), partialFileList.end());
        }
        return fileList;
    }

    bool AssetSystem::Resolve(const std::string &rRelFilePath, std::string &rAbsFilePath) const
    {
        for (const auto &bundleRoot : mBundleRoots)
        {
            auto absFilePath = bundleRoot + "/" + rRelFilePath;
            if (File::Exists(absFilePath))
            {
                rAbsFilePath = absFilePath;
                return true;
            }
        }
        return false;
    }

    std::string AssetSystem::Resolve(const std::string &rRelFilePath) const
    {
        std::string absFilePath{};
        Resolve(rRelFilePath, absFilePath);
        return absFilePath;
    }

    void AssetSystem::Expand(const std::string &rRelFilePath, std::vector<std::string> &rAbsFilePaths, bool includeNonExistant /* = false*/) const
    {
        rAbsFilePaths.clear();
        for (const auto &bundleRoot : mBundleRoots)
        {
            auto absFilePath = bundleRoot + "/" + rRelFilePath;
            if (includeNonExistant || File::Exists(absFilePath))
            {
                rAbsFilePaths.emplace_back(absFilePath);
            }
        }
    }

    std::vector<std::string> AssetSystem::Expand(const std::string &rRelFilePath, bool includeNonExistant /* = false*/) const
    {
        std::vector<std::string> absFilePaths;
        Expand(rRelFilePath, absFilePaths, includeNonExistant);
        return absFilePaths;
    }

}