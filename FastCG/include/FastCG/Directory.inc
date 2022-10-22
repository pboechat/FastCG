#include <FastCG/FastCG.h>
#include <FastCG/Exception.h>

#ifdef FASTCG_WINDOWS
#include <Windows.h>
#include <cstring>
#endif

#ifdef FASTCG_WINDOWS
namespace
{
	void List(const std::string &rDirectoryPath, bool recursive, std::vector<std::string> &fileList)
	{
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
					List(rDirectoryPath + "/" + fileName, true, fileList);
				}
				else
				{
					fileList.emplace_back(rDirectoryPath + "/" + fileName);
				}
			}
			FindClose(findHandle);
		}
	}
}
#endif

namespace FastCG
{
	bool Directory::Exists(const std::string &rDirectoryPath)
	{
#ifdef FASTCG_WINDOWS
		auto directoryAttributes = GetFileAttributes(rDirectoryPath.c_str());
		return (directoryAttributes != INVALID_FILE_ATTRIBUTES && (directoryAttributes & FILE_ATTRIBUTE_DIRECTORY));
#else
#error "FastCG::Directory::Exists() is not implemented on the current platform"
#endif
	}

	std::vector<std::string> Directory::List(const std::string &rDirectoryPath, bool recursive /* = false*/)
	{
#ifdef FASTCG_WINDOWS
		if (!Exists(rDirectoryPath))
		{
			return {};
		}

		std::vector<std::string> fileList;
		::List(rDirectoryPath, recursive, fileList);
		return fileList;
#else
#error "FastCG::Directory::ListFiles() is not implemented on the current platform"
#endif
	}

}