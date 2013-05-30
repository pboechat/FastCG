#include <Directory.h>
#include <Exception.h>

#ifdef _WIN32
#include <Windows.h>
#endif

std::vector<std::string> Directory::ListFiles(const std::string& rDirectoryPath)
{
#ifdef _WIN32
	std::vector<std::string> files;
	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile((rDirectoryPath + "\\*.*").c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			files.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	return files;
#else
	THROW_EXCEPTION(Exception, "Directory::ListFile() is not implemented in this platform");
#endif
}