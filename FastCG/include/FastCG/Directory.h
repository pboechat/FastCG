#ifndef FASTCG_DIRECTORY_H_
#define FASTCG_DIRECTORY_H_

#include <vector>
#include <string>

namespace FastCG
{
	class Directory
	{
	public:
		static bool Exists(const std::string& rDirectoryPath);
		static std::vector<std::string> ListFiles(const std::string& rDirectoryPath);

	private:
		Directory() = delete;
		~Directory() = delete;

	};

}

#endif