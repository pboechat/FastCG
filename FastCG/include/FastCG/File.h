#ifndef FASTCG_FILE_H
#define FASTCG_FILE_H

#include <string>
#include <initializer_list>

namespace FastCG
{
	class File
	{
	public:
		inline static std::string GetBasePath(const std::string &rFilePath);
		inline static std::string GetFileName(const std::string &rFilePath);
		inline static std::string GetFileNameWithoutExtension(const std::string &rFilePath);
		inline static bool Exists(const std::string &rFilePath);
		inline static std::string Join(const std::initializer_list<std::string> &rParts);

	private:
		File() = delete;
		~File() = delete;
	};

}

#include <FastCG/File.inc>

#endif