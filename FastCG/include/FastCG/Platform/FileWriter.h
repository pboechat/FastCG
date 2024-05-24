#ifndef FASTCG_FILE_WRITER_H
#define FASTCG_FILE_WRITER_H

#include <cstdint>
#include <string>

namespace FastCG
{
    class FileWriter final
    {
    public:
        inline static void WriteText(const std::string &rFileName, const char *pData, size_t dataSize);
        inline static void WriteBinary(const std::string &rFileName, const uint8_t *pData, size_t dataSize);

    private:
        FileWriter() = delete;
        ~FileWriter() = delete;
    };

}

#include <FastCG/Platform/FileWriter.inc>

#endif
