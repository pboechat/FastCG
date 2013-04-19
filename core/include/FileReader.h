#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <string>

enum FileMode
{
	FM_TEXT, FM_BINARY
};

char* GetFileModeString(FileMode mode);

class FileReader
{
public:
	static char* Read(const std::string& rFileName, FileMode mode);
	static char* Read(const std::string& rFileName, int& fileSize, FileMode mode);

private:
	FileReader();
	~FileReader();
};

#endif
