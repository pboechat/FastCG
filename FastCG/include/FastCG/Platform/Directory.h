#ifndef FASTCG_DIRECTORY_H
#define FASTCG_DIRECTORY_H

#include <vector>
#include <string>

namespace FastCG
{
	class Directory final
	{
	public:
		inline static bool Exists(const std::string &rDirectoryPath);
		inline static std::vector<std::string> List(const std::string &rDirectoryPath, bool recursive = false);

	private:
		Directory() = delete;
		~Directory() = delete;
	};

}

#include <FastCG/Platform/Directory.inc>

#endif