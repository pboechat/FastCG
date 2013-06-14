#include <Directory.h>
#include <Exception.h>

#ifdef _WIN32
#include <Windows.h>
#endif

bool Directory::Exists(const std::string& rDirectoryPath)
{
#ifdef _WIN32
	unsigned long directoryAttributes = GetFileAttributes(rDirectoryPath.c_str());
	return (directoryAttributes != INVALID_FILE_ATTRIBUTES && (directoryAttributes & FILE_ATTRIBUTE_DIRECTORY));
#else
	THROW_EXCEPTION(Exception, "Directory::Exists() is not implemented on this platform");
#endif
}

std::vector<std::string> Directory::ListFiles(const std::string& rDirectoryPath)
{
#ifdef _WIN32
	std::vector<std::string> files;

	if (!Exists(rDirectoryPath))
	{
		return files;
	}

	HANDLE searchHandle;
	WIN32_FIND_DATA data;

	searchHandle = FindFirstFile((rDirectoryPath + "\\*.*").c_str(), &data);
	if (searchHandle != INVALID_HANDLE_VALUE) {
		do {
			files.push_back(data.cFileName);
		} while (FindNextFile(searchHandle, &data));
		FindClose(searchHandle);
	}
	return files;
#else
	THROW_EXCEPTION(Exception, "Directory::ListFile() is not implemented on this platform");
#endif
}