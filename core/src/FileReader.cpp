#include <FileReader.h>
#include <IOExceptions.h>

#include <string.h>
//#include <iostream>
#include <stdlib.h>
#include <stdio.h>

FileReader::FileReader()
{
}

FileReader::~FileReader()
{
}

std::string FileReader::Read(const std::string& rFileName)
{
	int unusedFileSize;
	return Read(rFileName, unusedFileSize);
}

std::string FileReader::Read(const std::string& rFileName, int& fileSize)
{
	if (rFileName.empty())
	{
		fileSize = 0;
		return "";
	}

	FILE* fileHandle = fopen(rFileName.c_str(), "rt");

	if (fileHandle == 0)
	{
		THROW_EXCEPTION(FileNotFoundException, "File not found: %s", rFileName.c_str());
	}

	fseek(fileHandle, 0, SEEK_END);
	fileSize = ftell(fileHandle);
	rewind(fileHandle);

	char* pBuffer = 0;
	int bytesRead = fileSize;
	if (bytesRead > 0)
	{
		pBuffer = (char*) malloc(sizeof(char) * (bytesRead + 1));
		bytesRead = fread(pBuffer, sizeof(char), bytesRead, fileHandle);
		pBuffer[bytesRead] = '\0';
	}

	fclose(fileHandle);

	std::string content(pBuffer);
	free(pBuffer);

	return content;
}
