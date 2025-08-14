#ifndef FASTCG_FILE_READER_H
#define FASTCG_FILE_READER_H

#include <filesystem>
#include <memory>
#include <string>

namespace FastCG
{
    class FileReader final
    {
    public:
        inline static std::unique_ptr<char[]> ReadText(const std::filesystem::path &rFilePath, size_t &rFileSize);
        inline static std::unique_ptr<uint8_t[]> ReadBinary(const std::filesystem::path &rFilePath, size_t &rFileSize);

    private:
        FileReader() = delete;
        ~FileReader() = delete;
    };

}

#include <FastCG/Platform/FileReader.inc>

#endif
