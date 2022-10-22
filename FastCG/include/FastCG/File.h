#ifndef FASTCG_FILE_H
#define FASTCG_FILE_H

#include <string>

namespace FastCG
{
	class File
	{
	public:
		inline static std::string GetBasePath(const std::string &rFilePath);
		inline static std::string GetFileName(const std::string &rFilePath);
		inline static std::string GetFileNameWithoutExtension(const std::string &rFilePath);
		inline static bool Exists(const std::string &rFilePath);

	private:
		File() = delete;
		~File() = delete;
	};

}

#include <FastCG/File.inc>

#endif