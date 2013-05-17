#include <Directory.h>

#ifdef _WIN32
#include <Windows.h>
#endif

std::vector<std::string> Directory::ListFiles(const std::string& rDirectoryPath)
{
	std::vector<std::string> files;
#ifdef _WIN32
	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile((rDirectoryPath + "\\*.*").c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			files.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
#endif

	return files;
}