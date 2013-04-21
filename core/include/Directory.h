#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <vector>
#include <string>

class Directory
{
public:
	static std::vector<std::string> ListFiles(const std::string& rDirectoryPath);

private:
	Directory()
	{
	}

	~Directory()
	{
	}
};

#endif