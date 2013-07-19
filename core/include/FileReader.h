#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <string>

class FileReader
{
public:
	static std::string Read(const std::string& rFileName);
	static std::string Read(const std::string& rFileName, int& fileSize);

private:
	FileReader();
	~FileReader();
};

#endif
