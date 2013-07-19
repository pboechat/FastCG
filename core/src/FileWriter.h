#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include <string>

class FileWriter
{
public:
	static void WriteText(const std::string& rFileName, const std::string& rText);
	static void WriteBytes(const std::string& rFileName, int size, const unsigned char* pData);

private:
	FileWriter();
	~FileWriter();
};

#endif
