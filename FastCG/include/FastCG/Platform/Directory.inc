#if defined FASTCG_WINDOWS
#include <Windows.h>
#include <cstring>
#elif defined FASTCG_POSIX
#include <sys/stat.h>
#if defined FASTCG_POSIX
#include <dirent.h>
#endif
#endif

namespace
{
	void List(const std::string &rDirectoryPath, bool recursive, std::vector<std::string> &rFileList)
	{
#if defined FASTCG_WINDOWS
		WIN32_FIND_DATA findData;
		auto findHandle = FindFirstFile((rDirectoryPath + "\\*.*").c_str(), &findData);
		if (findHandle != INVALID_HANDLE_VALUE)
		{
			while (FindNextFile(findHandle, &findData))
			{
				auto *fileName = findData.cFileName;
				if (strcmp(fileName, "..") == 0)
				{
					continue;
				}
				if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && recursive)
				{
					List(rDirectoryPath + "/" + fileName, true, rFileList);
				}
				else
				{
					rFileList.emplace_back(rDirectoryPath + "/" + fileName);
				}
			}
			FindClose(findHandle);
		}
#elif defined FASTCG_POSIX
		if (auto dir = opendir(rDirectoryPath.c_str()))
		{
			while (auto file = readdir(dir))
			{
				if (!file->d_name[0] || file->d_name[0] == '.')
				{
					continue;
				}
				if (file->d_type == DT_DIR && recursive)
				{
					List(rDirectoryPath + "/" + file->d_name, true, rFileList);
				}
				else if (file->d_type == DT_REG)
				{
					rFileList.emplace_back(rDirectoryPath + "/" + file->d_name);
				}
			}
			closedir(dir);
		}
#else
#error "::List() is not implemented on the current platform"
#endif
	}
}

namespace FastCG
{
	bool Directory::Exists(const std::string &rDirectoryPath)
	{
#if defined FASTCG_WINDOWS
		auto directoryAttributes = GetFileAttributes(rDirectoryPath.c_str());
		return (directoryAttributes != INVALID_FILE_ATTRIBUTES && (directoryAttributes & FILE_ATTRIBUTE_DIRECTORY));
#elif defined FASTCG_POSIX
		struct stat fileStat;
		return stat(rDirectoryPath.c_str(), &fileStat) == 0 && (fileStat.st_mode & S_IFDIR) != 0;
#else
#error "FastCG::Directory::Exists() is not implemented on the current platform"
#endif
	}

	std::vector<std::string> Directory::List(const std::string &rDirectoryPath, bool recursive /* = false*/)
	{
		if (!Exists(rDirectoryPath))
		{
			return {};
		}

		std::vector<std::string> fileList;
		::List(rDirectoryPath, recursive, fileList);
		return fileList;
	}

}