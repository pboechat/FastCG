namespace FastCG
{
    std::vector<std::filesystem::path> AssetSystem::List(const std::filesystem::path &rRelDirectoryPath,
                                                         bool recursive /* = false*/) const
    {
        std::vector<std::filesystem::path> fileList;
        for (const auto &rBundleRoot : mBundleRoots)
        {
            auto absDirPath = rBundleRoot / rRelDirectoryPath;
            if (std::filesystem::exists(absDirPath) && std::filesystem::is_directory(absDirPath))
            {
                if (recursive)
                {
                    for (const auto &rEntry : std::filesystem::recursive_directory_iterator(absDirPath))
                    {
                        auto filePath = rEntry.path();
                        if (std::filesystem::is_directory(filePath))
                        {
                            continue;
                        }
                        fileList.push_back(filePath);
                    }
                }
                else
                {
                    for (const auto &rEntry : std::filesystem::directory_iterator(absDirPath))
                    {
                        auto filePath = rEntry.path();
                        if (std::filesystem::is_directory(filePath))
                        {
                            continue;
                        }
                        fileList.push_back(filePath);
                    }
                }
            }
        }
        return fileList;
    }

    bool AssetSystem::Resolve(const std::filesystem::path &rRelFilePath, std::filesystem::path &rAbsFilePath) const
    {
        for (const auto &rBundleRoot : mBundleRoots)
        {
            auto absFilePath = rBundleRoot / rRelFilePath;
            if (std::filesystem::exists(absFilePath))
            {
                rAbsFilePath = absFilePath;
                return true;
            }
        }
        return false;
    }

    std::filesystem::path AssetSystem::Resolve(const std::filesystem::path &rRelFilePath) const
    {
        std::filesystem::path absFilePath;
        Resolve(rRelFilePath, absFilePath);
        return absFilePath;
    }

    void AssetSystem::Expand(const std::filesystem::path &rRelFilePath,
                             std::vector<std::filesystem::path> &rAbsFilePaths,
                             bool includeNonExistant /* = false*/) const
    {
        rAbsFilePaths.clear();
        for (const auto &rBundleRoot : mBundleRoots)
        {
            auto absFilePath = rBundleRoot / rRelFilePath;
            if (includeNonExistant || std::filesystem::exists(absFilePath))
            {
                rAbsFilePaths.emplace_back(absFilePath);
            }
        }
    }

    std::vector<std::filesystem::path> AssetSystem::Expand(const std::filesystem::path &rRelFilePath,
                                                           bool includeNonExistant /* = false*/) const
    {
        std::vector<std::filesystem::path> absFilePaths;
        Expand(rRelFilePath, absFilePaths, includeNonExistant);
        return absFilePaths;
    }

}