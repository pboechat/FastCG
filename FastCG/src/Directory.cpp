#include <FastCG/FastCG.h>
#include <FastCG/Exception.h>
#include <FastCG/Directory.h>

#ifdef FASTCG_WINDOWS
#include <Windows.h>
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

	std::vector<std::string> Directory::ListFiles(const std::string &rDirectoryPath)
	{
#ifdef FASTCG_WINDOWS
		std::vector<std::string> files;

		if (!Exists(rDirectoryPath))
		{
			return files;
		}

		HANDLE searchHandle;
		WIN32_FIND_DATA data;

		searchHandle = FindFirstFile((rDirectoryPath + "\\*.*").c_str(), &data);
		if (searchHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				files.emplace_back(data.cFileName);
			} while (FindNextFile(searchHandle, &data));
			FindClose(searchHandle);
		}
		return files;
#else
#error "FastCG::Directory::ListFiles() is not implemented on the current platform"
#endif
	}

}