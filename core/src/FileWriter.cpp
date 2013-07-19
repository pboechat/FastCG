#include "FileWriter.h"
#include "Exception.h"

#include <string.h>
//#include <iostream>
#include <stdlib.h>
#include <stdio.h>

FileWriter::FileWriter()
{
}

FileWriter::~FileWriter()
{
}

void FileWriter::WriteText(const std::string& rFileName, const std::string& rText)
{
	if (rFileName.empty())
	{
		return;
	}

	FILE* fileHandle = fopen(rFileName.c_str(), "wt");

	if (fileHandle == 0)
	{
		THROW_EXCEPTION(Exception, "Cannot write to file: %s", rFileName.c_str());
	}

	int bytesToWrite = rText.size() * sizeof(char);
	int bytesWritten = fwrite((const void*)rText.c_str(), sizeof(char), rText.size(), fileHandle);

	if (bytesWritten != bytesToWrite)
	{
		THROW_EXCEPTION(Exception, "Error writting to file: %s", rFileName.c_str());
	}

	fclose(fileHandle);
}

void FileWriter::WriteBytes(const std::string& rFileName, int size, const unsigned char* pData)
{
	if (rFileName.empty())
	{
		return;
	}

	FILE* fileHandle = fopen(rFileName.c_str(), "wb");

	if (fileHandle == 0)
	{
		THROW_EXCEPTION(Exception, "Cannot write to file: %s", rFileName.c_str());
	}

	int bytesToWrite = size * sizeof(unsigned char);
	int bytesWritten = fwrite((const void*)pData, sizeof(unsigned char), size, fileHandle);

	if (bytesWritten != bytesToWrite)
	{
		THROW_EXCEPTION(Exception, "Error writting to file: %s", rFileName.c_str());
	}

	fclose(fileHandle);
}
