#ifndef FILE_H_
#define FILE_H_

#include <string>

class File
{
public:
	static std::string GetPath(const std::string& rFilePath);
	static std::string GetFileName(const std::string& rFilePath);

private:
	File()
	{
	}

	~File()
	{
	}

};

#endif