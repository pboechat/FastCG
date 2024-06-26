#ifndef FASTCG_FILE_H
#define FASTCG_FILE_H

#include <initializer_list>
#include <string>

namespace FastCG
{
    class File final
    {
    public:
        inline static std::string GetBasePath(const std::string &rFilePath);
        inline static std::string GetFileName(const std::string &rFilePath);
        inline static void SplitExt(const std::string &rFilePath, std::string &rFileName, std::string &rExtension);
        inline static std::string GetFileNameWithoutExtension(const std::string &rFilePath);
        inline static bool Exists(const std::string &rFilePath);
        inline static std::string Join(const std::initializer_list<std::string> &rParts);

    private:
        File() = delete;
        ~File() = delete;
    };

}

#include <FastCG/Platform/File.inc>

#endif