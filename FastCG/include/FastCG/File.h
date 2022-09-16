#ifndef FASTCG_FILE_H_
#define FASTCG_FILE_H_

#include <string>

namespace FastCG
{
	class File
	{
	public:
		static std::string GetFilePath(const std::string& rFilePath);
		static std::string GetFileName(const std::string& rFilePath);
		static std::string GetFileNameWithoutExtension(const std::string& rFilePath);
		static bool Exists(const std::string& rFilePath);

	private:
		File() = delete;
		~File() = delete;

	};

}

#endif