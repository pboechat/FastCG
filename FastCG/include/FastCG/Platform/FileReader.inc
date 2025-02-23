#include <fstream>

namespace
{
    template <typename T>
    std::unique_ptr<T[]> Read(const std::filesystem::path &rFilePath, size_t &rFileSize)
    {
        if (!std::filesystem::exists(rFilePath))
        {
            rFileSize = 0;
            return nullptr;
        }

        std::ifstream fileStream(rFilePath, std::ios_base::in | std::ios_base::binary);
        if (!fileStream.is_open())
        {
            rFileSize = 0;
            return nullptr;
        }

        fileStream.seekg(0, std::ios_base::end);
        rFileSize = fileStream.tellg();
        fileStream.seekg(0, std::ios_base::beg);

        std::unique_ptr<T[]> buffer;
        if (rFileSize > 0)
        {
            buffer = std::unique_ptr<T[]>(new T[rFileSize]);
            fileStream.read((char *)buffer.get(), rFileSize);
        }

        fileStream.close();

        return buffer;
    }

}

namespace FastCG
{
    std::unique_ptr<char[]> FileReader::ReadText(const std::filesystem::path &rFilePath, size_t &rFileSize)
    {
        return Read<char>(rFilePath, rFileSize);
    }

    std::unique_ptr<uint8_t[]> FileReader::ReadBinary(const std::filesystem::path &rFilePath, size_t &rFileSize)
    {
        return Read<uint8_t>(rFilePath, rFileSize);
    }

}